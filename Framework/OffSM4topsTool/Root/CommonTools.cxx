#include "OffSM4topsTool/CommonTools.h"
#include <TFile.h>
#include <TH1F.h>
#include <fstream>
#include "OffSM4topsTool/OffSM4topsConfiguration.h"
#include "PathResolver/PathResolver.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"

namespace TTHbb {

// ----------------------------- public -----------------------------------------

void CommonTools::initialise() {
  // Check if input ntuple is data
  isData = false;
  auto* config = TTHbb::GlobalConfiguration::get();
  if (TTHbb::util::iequals((*config)("SampleType"), "data")) isData = true;

  auto* config4Tops = (OffSM4topsConfiguration*)TTHbb::OffSM4topsConfiguration::get();

  // Weight for the combination of ttbar samples
  m_ttbarStitchingDsidMapPath =
      PathResolverFindCalibFile("OffSM4topsTool/ttbarStitchingDsidMap.json");
  if (!m_ttbarStitchingDsidMapPath.empty()) {
    std::ifstream jsonFile(m_ttbarStitchingDsidMapPath);
    jsonFile >> m_ttbarStitchingDsidMap;

    std::cout << "CommonTools: Using ttbar stitching:" << std::endl;
    for (nlohmann::json::iterator it = m_ttbarStitchingDsidMap.begin();
         it != m_ttbarStitchingDsidMap.end(); ++it) {
      std::cout << "CommonTools: " << it.key() << " : " << it.value() << std::endl;
    }
  } else {
    std::cerr
        << "CommonTools: ttbar stitching config not found by PathResolver. setting weights to 1."
        << std::endl;
  }
}

void CommonTools::apply(TTHbb::Event* event) {
  if (!m_ttbarStitchingDsidMapPath.empty()) AddTtbarCombinationWeight(event);
}

void CommonTools::finalise() {}

// Weight for the combination of ttbar samples
void CommonTools::AddTtbarCombinationWeight(TTHbb::Event* event) {
  auto mcChannelNumber = (int)(event->m_info->mcChannelNumber);
  auto topHeavyFlavorFilterFlag = event->intVariable("TopHeavyFlavorFilterFlag");
  auto genFiltHt = event->floatVariable("GenFiltHT");
  if (topHeavyFlavorFilterFlag < 0)
    std::cerr << "CommonTools: TopHeavyFlavorFilterFlag not properly filled. Please change your "
                 "VariablesToRead file."
              << std::endl;
  float wTtbarInclHTSlicedHFFiltered =
      WeightTtbarInclHTSlicedHFFiltered(mcChannelNumber, genFiltHt, topHeavyFlavorFilterFlag);

  event->floatVariable("weight_ttbarInclHTSlicedHFFiltered") = wTtbarInclHTSlicedHFFiltered;
}

// Code base from Romain, see: https://gitlab.cern.ch/atlasphys-top/xs/4tops/intnote/issues/30.
// Slight modifications by Lennart. Naming is pythonic.
float CommonTools::WeightTtbarInclHTSlicedHFFiltered(int dsid, float truth_ht,
                                                     int flavor_filter_flag) {
  if (m_ttbarStitchingDsidMapPath.empty()) return 1;

  std::vector<int> ttbar_incl_dsid = {};
  std::vector<int> ttbar_htsliced_dsid = {};
  std::vector<int> ttbar_bbfilter_dsid = {};
  std::vector<int> ttbar_bfilterBBVeto_dsid = {};
  std::vector<int> ttbar_cfilterBVeto_dsid = {};

  if (m_ttbarStitchingDsidMap.find("ttbar_incl_dsid") != m_ttbarStitchingDsidMap.end())
    ttbar_incl_dsid = m_ttbarStitchingDsidMap["ttbar_incl_dsid"].get<std::vector<int>>();
  if (m_ttbarStitchingDsidMap.find("ttbar_htsliced_dsid") != m_ttbarStitchingDsidMap.end())
    ttbar_htsliced_dsid = m_ttbarStitchingDsidMap["ttbar_htsliced_dsid"].get<std::vector<int>>();
  if (m_ttbarStitchingDsidMap.find("ttbar_bbfilter_dsid") != m_ttbarStitchingDsidMap.end())
    ttbar_bbfilter_dsid = m_ttbarStitchingDsidMap["ttbar_bbfilter_dsid"].get<std::vector<int>>();
  if (m_ttbarStitchingDsidMap.find("ttbar_bfilterBBVeto_dsid") != m_ttbarStitchingDsidMap.end())
    ttbar_bfilterBBVeto_dsid = m_ttbarStitchingDsidMap["ttbar_bfilterBBVeto_dsid"].get<std::vector<int>>();
  if (m_ttbarStitchingDsidMap.find("ttbar_cfilterBVeto_dsid") != m_ttbarStitchingDsidMap.end())
    ttbar_cfilterBVeto_dsid =
        m_ttbarStitchingDsidMap["ttbar_cfilterBVeto_dsid"].get<std::vector<int>>();

  // Lennart: Beware that a binary search requires the vectors to be in ascending order!!
  // Seeing that those vectors are very small, I changed the `search` to a `find` for stability.

  bool is_ttbar_incl =
      std::find(ttbar_incl_dsid.begin(), ttbar_incl_dsid.end(), dsid) != ttbar_incl_dsid.end();
  bool is_ttbar_htsl = std::find(ttbar_htsliced_dsid.begin(), ttbar_htsliced_dsid.end(), dsid) !=
                       ttbar_htsliced_dsid.end();
  bool is_ttbar_bb = std::find(ttbar_bbfilter_dsid.begin(), ttbar_bbfilter_dsid.end(), dsid) !=
                     ttbar_bbfilter_dsid.end();
  bool is_ttbar_b = std::find(ttbar_bfilterBBVeto_dsid.begin(), ttbar_bfilterBBVeto_dsid.end(), dsid) !=
                    ttbar_bfilterBBVeto_dsid.end();
  bool is_ttbar_c = std::find(ttbar_cfilterBVeto_dsid.begin(), ttbar_cfilterBVeto_dsid.end(),
                              dsid) != ttbar_cfilterBVeto_dsid.end();
  bool is_lowHT = truth_ht < 600 * 1000;
  bool is_highHT1 = 600 * 1000 <= truth_ht && truth_ht < 1000 * 1000;
  bool is_highHT2 = 1000 * 1000 <= truth_ht && truth_ht < 1500 * 1000;
  bool is_highHT3 = 1500 * 1000 <= truth_ht;

  // Preparing result
  float w(-1.);
  // Temporary -> should be computed beforehand
  float frac_ht1_HFflag1(0.5), frac_ht2_HFflag1(0.5), frac_ht3_HFflag1(0.5);
  float frac_ht1_HFflag2(0.5), frac_ht2_HFflag2(0.5), frac_ht3_HFflag2(0.5);
  float frac_ht1_HFflag3(0.5), frac_ht2_HFflag3(0.5), frac_ht3_HFflag3(0.5);
  // HFflag==0 || lowHT: take only inclusive/HT-sliced/HF-filtered samples
  if (flavor_filter_flag == 0 && is_lowHT) {
    if (is_ttbar_incl)
      w = 1.0;
    else
      w = 0.0;
  } else if (flavor_filter_flag == 0 && !is_lowHT) {
    if (is_ttbar_htsl)
      w = 1.0;
    else
      w = 0.0;
  } else if (flavor_filter_flag > 0 && is_lowHT) {
    if (is_ttbar_bb && flavor_filter_flag == 1)
      w = 1.0;
    else if (is_ttbar_b && flavor_filter_flag == 2)
      w = 1.0;
    else if (is_ttbar_c && flavor_filter_flag == 3)
      w = 1.0;
    else
      w = 0.0;
  }
  // By security to ensure orhtogonality - not need with the proper truth_HT def
  else if (is_ttbar_htsl && is_lowHT)
    w = 0.0;

  // HighHT1/2/3, HFflag==1: take a weighted combination of HT-sliced and bb-filtered if not lowHT
  else if (!is_lowHT && flavor_filter_flag == 1) {
    if (is_ttbar_htsl) {
      if (is_highHT1)
        w = frac_ht1_HFflag1;
      else if (is_highHT2)
        w = frac_ht2_HFflag1;
      else if (is_highHT3)
        w = frac_ht3_HFflag1;
      else
        w = 0.0;
    } else if (is_ttbar_bb) {
      if (is_highHT1)
        w = 1 - frac_ht1_HFflag1;
      else if (is_highHT2)
        w = 1 - frac_ht2_HFflag1;
      else if (is_highHT3)
        w = 1 - frac_ht3_HFflag1;
      else
        w = 0.0;
    } else
      w = 0.0;
  }
  // HighHT1/2/3, HFflag==2: take a weighted combination of HT-sliced and b-filtered if not lowHT
  else if (!is_lowHT && flavor_filter_flag == 2) {
    if (is_ttbar_htsl) {
      if (is_highHT1)
        w = frac_ht1_HFflag2;
      else if (is_highHT2)
        w = frac_ht2_HFflag2;
      else if (is_highHT3)
        w = frac_ht3_HFflag2;
      else
        w = 0.0;
    } else if (is_ttbar_b) {
      if (is_highHT1)
        w = 1 - frac_ht1_HFflag2;
      else if (is_highHT2)
        w = 1 - frac_ht2_HFflag2;
      else if (is_highHT3)
        w = 1 - frac_ht3_HFflag2;
      else
        w = 0.0;
    } else
      w = 0.0;
  }
  // HighHT1/2/3, HFflag==3: take a weighted combination of HT-sliced and c-filtered if not lowHT
  else if (!is_lowHT && flavor_filter_flag == 3) {
    if (is_ttbar_htsl) {
      if (is_highHT1)
        w = frac_ht1_HFflag3;
      else if (is_highHT2)
        w = frac_ht2_HFflag3;
      else if (is_highHT3)
        w = frac_ht3_HFflag3;
      else
        w = 0.0;
    } else if (is_ttbar_c) {
      if (is_highHT1)
        w = 1 - frac_ht1_HFflag3;
      else if (is_highHT2)
        w = 1 - frac_ht2_HFflag3;
      else if (is_highHT3)
        w = 1 - frac_ht3_HFflag3;
      else
        w = 0.0;
    } else
      w = 0.0;
  }

  else
    w = 1.0;
  return w;
}
}  // namespace TTHbb
