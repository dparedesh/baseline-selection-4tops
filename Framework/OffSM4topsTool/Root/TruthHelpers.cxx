#include "OffSM4topsTool/TruthHelpers.h"

namespace SM4t {

// Fakes categories helper functions (copied from
// https://gitlab.cern.ch/ATLAS-IFF/IFFTruthClassifier/blob/master/Root/IFFTruthClassifier.cxx)
// TODO? Include the official IFF tool in the online code?
bool TruthHelpers::isFromPromptSource(int origin) {
  return origin == 2 ||   // SingleMuon
         origin == 10 ||  // top
         origin == 12 ||  // WBoson
         origin == 13 ||  // ZBoson
         origin == 14 ||  // Higgs
         origin == 15 ||  // HiggsMSSM
         origin == 22 ||  // SUSY
         origin == 43;    // DiBoson
}
bool TruthHelpers::isFromSMPromptSource(int origin) {
  return origin == 10 ||  // top
         origin == 12 ||  // WBoson
         origin == 13 ||  // ZBoson
         origin == 14 ||  // Higgs
         origin == 43;    // DiBoson
}
bool TruthHelpers::isFromLightSource(int origin) {
  return origin == 23 ||  // LightMeson
         origin == 24 ||  // StrangeMeson
         origin == 30 ||  // LightBaryon
         origin == 31 ||  // StrangeBaryon
         origin == 34 ||  // PionDecay
         origin == 35;    // KaonDecay
}
bool TruthHelpers::hasBHadronOrigin(int origin) {
  return origin == 26 ||  // BottomMeson
         origin == 29 ||  // BBbarMeson
         origin == 33;    // BottomBaryon
}
bool TruthHelpers::hasCHadronOrigin(int origin) {
  return origin == 25 ||  // CharmedMeson
         origin == 27 ||  // CCbarMeson
         origin == 32;    // CharmedBaryon
  // No J/psi???
}
bool TruthHelpers::isPromptElectron(int type, int origin, int motherPdgid, int motherType, int motherOrigin) {
  bool isPrompt = false;  // init
  bool hasElectronMother = ((motherPdgid == 11) || (motherPdgid == -11));

  if (type == 2)
    isPrompt = true;
  else if (type == 4 && origin == 5 && hasElectronMother)
    isPrompt = true;
  else if (type == 4 && origin == 7 && hasElectronMother and motherType == 2 && isFromSMPromptSource(motherOrigin))
    isPrompt = true;
  else
    isPrompt = false;

  return isPrompt;
}
bool TruthHelpers::isChargeFlip(int charge, int motherPdgid) {
  return (charge * motherPdgid) > 0;
  // True charge != reco charge.
  // TODO: this requirement should be OK for prompt electrons, but maybe not for type 4 and origin
  // 5?? ==> check the sign of el_firstEgMotherPdgId only if the firstEgMother is an electron (11 of
  // -11)?? Else charge undefined?
}

std::bitset<7> TruthHelpers::leptonCategorisationIFF(std::shared_ptr<TTHbb::Lepton> lep) {
  /* Bitword is:
  isFromPromptSource = 0,
  isFromSMPromptSource = 1,
  isFromLightSource = 2,
  hasBHadronOrigin = 3,
  hasCHadronOrigin = 4,
  isPromptElectron = 5,
  isChargeFlip = 6
  */

  int truthType, truthOrigin = -1;
  std::bitset<7> res;
  res.reset();

  if (lep->type() == TTHbb::leptonType::electron) {
    truthType = lep->intVariable("el_true_type");
    truthOrigin = lep->intVariable("el_true_origin");
    int motherTruthType =
        lep->intVariable("el_true_firstEgMotherTruthType");  // These names differ from those at master (2019-02-05)
    int motherTruthOrigin =
        lep->intVariable("el_true_firstEgMotherTruthOrigin");  // These names differ from those at master (2019-02-05)
    int motherTruthPdgid =
        lep->intVariable("el_true_firstEgMotherPdgId");  // These names differ from those at master (2019-02-05)

    res.set(5, isPromptElectron(truthType, truthOrigin, motherTruthPdgid, motherTruthType, motherTruthOrigin));
    res.set(6, isChargeFlip(lep->charge(), motherTruthPdgid));
  }

  else if (lep->type() == TTHbb::leptonType::muon) {
    truthType = lep->intVariable("el_true_type");
    truthOrigin = lep->intVariable("el_true_origin");
  }

  res.set(0, isFromPromptSource(truthOrigin));
  res.set(1, isFromSMPromptSource(truthOrigin));
  res.set(2, isFromLightSource(truthOrigin));
  res.set(3, hasBHadronOrigin(truthOrigin));
  res.set(4, hasCHadronOrigin(truthOrigin));

  // std::cout << res << " | " << (res.to_ulong() & (1 << 0)) << " | " << isFromPromptSource(truthOrigin) <<std::endl;
  return res;
}

int TruthHelpers::electronCategorisationZhi(std::shared_ptr<TTHbb::Lepton> el) {
  int truthType = el->intVariable("el_true_type");
  int truthOrigin = el->intVariable("el_true_origin");
  int motherTruthType =
      el->intVariable("el_true_firstEgMotherTruthType");  // These names differ from those at master (2019-02-05)
  int motherTruthOrigin =
      el->intVariable("el_true_firstEgMotherTruthOrigin");  // These names differ from those at master (2019-02-05)
  int motherTruthPdgid =
      el->intVariable("el_true_firstEgMotherPdgId");  // These names differ from those at master (2019-02-05)

  // std::cout << truthType << ", " << truthOrigin << ", " << motherTruthType << ", " << motherTruthOrigin << ", " <<
  // motherTruthPdgid  << std::endl;

  int fakeCateg = -1;

  // TODO: This could really do with a clean up
  if (isPromptElectron(truthType, truthOrigin, motherTruthPdgid, motherTruthType, motherTruthOrigin)) {
    if (isChargeFlip(el->charge(), motherTruthPdgid))
      fakeCateg = 1;  // charge flip electron
    else
      fakeCateg = 0;  // iso electron
  } else if (truthType == 4 && (truthOrigin == 5 || truthOrigin == 7) &&
             (motherTruthOrigin == 39 || motherTruthOrigin == 40))
    fakeCateg = 3;  // iso electron
  else if (truthType == 15 && (motherTruthOrigin == 39 || motherTruthOrigin == 40))
    fakeCateg = 3;  // iso electron. This is a photon though?
  else if ((truthType == 3 || truthType == 15) && truthOrigin == 9)
    fakeCateg = 7;  // tau decay
  else if (truthType == 4 && truthOrigin == 5 && motherTruthType == 15 && motherTruthOrigin == 9)
    fakeCateg = 7;  // tau decay
  else if (truthType == 4 && (truthOrigin == 5 || truthOrigin == 7) && motherTruthType == 4 && motherTruthOrigin == 5)
    fakeCateg = 2;  // internal conversion from Zhi
  else if (truthType == 4 && (truthOrigin == 5 || truthOrigin == 7) && motherTruthType == 14 && motherTruthOrigin == 37)
    fakeCateg = 3;  // prompt photon conversion
  else if (truthType == 4 && truthOrigin == 5 && motherTruthType == 14 && motherTruthOrigin == 14)
    fakeCateg = 3;  // prompt photon conversion
  else if (truthType == 4 && truthOrigin == 7 && motherTruthType == 16 && motherTruthOrigin == 38)
    fakeCateg = 3;  // prompt photon conversion
  else if (truthType == 14 && truthOrigin == 37)
    fakeCateg = 3;  // is a photon
  else if (truthType == 16 && truthOrigin == 38)
    fakeCateg = 3;  // is a photon
  else if (!isChargeFlip(el->charge(), motherTruthPdgid) && truthType == 4 && truthOrigin == 5 &&
           motherTruthType == 16 && motherTruthOrigin == 38)
    fakeCateg = 3;  // not sure?
  else if (truthType == 4 && truthOrigin == 5 && motherTruthType == 0 && motherTruthOrigin == 13) {
    std::cerr << "Electron identified as from a PromptPhotonConversion, but this type of electron needs further study!"
              << std::endl;
    fakeCateg = 3;  // not sure? electrons from muons?
  } else if (truthType == 17)
    fakeCateg = 6;  // light hadron decay
  else if (truthType == 4 && (truthOrigin == 6 || truthOrigin == 23 || truthOrigin == 24))
    fakeCateg = 6;  // light hadron decay
  else if (truthType == 4 && truthOrigin == 5 && motherTruthType == 16 &&
           (motherTruthOrigin == 42 || motherTruthOrigin == 23 || motherTruthOrigin == 24))
    fakeCateg = 6;  // light hadron decay
  else if (truthType == 4 && truthOrigin == 7 &&
           ((motherTruthType == 4 && motherTruthOrigin == 24) ||
            (motherTruthType == 16 && (motherTruthOrigin == 23 || motherTruthOrigin == 42))))
    fakeCateg = 6;  // light hadron decay
  else if (truthType == 4 && (truthOrigin == 42 || truthOrigin == 23 || truthOrigin == 24))
    fakeCateg = 6;  // light hadron decay
  else if (truthType == 3 && hasBHadronOrigin(truthOrigin))
    fakeCateg = 5;  // B-hadron decay
  else if (truthType == 4 && truthOrigin == 7 && (motherTruthType == 3 || motherTruthType == 16) &&
           motherTruthOrigin == 26)
    fakeCateg = 5;  // B-hadron decay
  else if (truthType == 4 && truthOrigin == 5 && motherTruthType == 16 &&
           (motherTruthOrigin == 26 || motherTruthOrigin == 33))
    fakeCateg = 5;  // B-hadron decay
  else if (truthType == 3 && hasCHadronOrigin(truthOrigin))
    fakeCateg = 5;  // C-hadron decay
  else if (truthType == 4 && truthOrigin == 27)
    fakeCateg = 5;  // C-hadron decay
  else if (truthType == 4 && truthOrigin == 5 && motherTruthType == 16 && hasCHadronOrigin(motherTruthOrigin))
    fakeCateg = 5;  // C-hadron decay
  else
    fakeCateg = 8;  // unknown?

  return fakeCateg;
}

int TruthHelpers::muonCategorisationZhi(std::shared_ptr<TTHbb::Lepton> mu) {
  int truthType = mu->intVariable("mu_true_type");
  int truthOrigin = mu->intVariable("mu_true_origin");
  int fakeCateg = -1;

  if (truthType == 6 && isFromPromptSource(truthOrigin))
    fakeCateg = 0;  // prompt muon
  else if ((truthType == 6 || truthType == 7) && hasBHadronOrigin(truthOrigin))
    fakeCateg = 5;  // B-hadron decay
  else if (truthType == 7 && hasCHadronOrigin(truthOrigin))
    fakeCateg = 5;  // C-hadron decay
  else if ((truthType == 6 || truthType == 8) && hasCHadronOrigin(truthOrigin))
    fakeCateg = 5;  // C-hadron decay. No J/Psi?
  else if (truthType == 8 && isFromLightSource(truthOrigin))
    fakeCateg = 6;  // light flavour decay
  else if (truthType == 17)
    fakeCateg = 6;  // light flavour decay
  else if (truthType == 7 && truthOrigin == 9)
    fakeCateg = 7;  // tau decay
  else if (truthType == 5 && truthOrigin == 0)
    fakeCateg = 4;  // known unknown (from Zhi)
  else if (truthType == 0 && truthOrigin == 0)
    fakeCateg = 4;  // known unknown (from Zhi)
  else
    fakeCateg = 8;  // unknown

  return fakeCateg;
}

int TruthHelpers::leptonCategorisationZhi(std::shared_ptr<TTHbb::Lepton> lep) {
  switch (lep->type()) {
    case TTHbb::leptonType::electron:
      return electronCategorisationZhi(lep);
      break;
    case TTHbb::leptonType::muon:
      return muonCategorisationZhi(lep);
      break;
    default:
      return -2;
  }
}

TruthHelpers::SM4tTruthCategoryFlag TruthHelpers::electronBkgFlagSM4t(std::shared_ptr<TTHbb::Lepton> el) {
  int truthType = el->intVariable("el_true_type");
  int truthOrigin = el->intVariable("el_true_origin");
  int motherTruthType = el->intVariable("el_true_firstEgMotherTruthType");
  int motherTruthOrigin = el->intVariable("el_true_firstEgMotherTruthOrigin");
  int motherPDGId = el->intVariable("el_true_firstEgMotherPdgId");
  bool isQFlip = isChargeFlip(el->charge(), motherPDGId);


  if (truthType == 17) return TruthHelpers::SM4tTruthCategoryFlag::Fake; //light-quark/gluons
  if (!(truthType >= 2 && truthType <= 4)) return TruthHelpers::SM4tTruthCategoryFlag::Other;

  if ((truthOrigin >= 9 && truthOrigin <= 14) || truthOrigin == 43) return isQFlip ? TruthHelpers::SM4tTruthCategoryFlag::PromptFlipQ : TruthHelpers::SM4tTruthCategoryFlag::Prompt;
  if (truthOrigin == 5) {
    if (motherTruthType == 2 && ((motherTruthOrigin >= 9 && motherTruthOrigin <= 14) || motherTruthOrigin == 43))
      return isQFlip ? TruthHelpers::SM4tTruthCategoryFlag::TridentFlipQ : TruthHelpers::SM4tTruthCategoryFlag::Trident;
    if ((motherTruthType == 14 && motherTruthOrigin == 37) || (motherTruthType == 15 && motherTruthOrigin == 40) ||
        ((motherTruthType == 15 || motherTruthType == 16) && (motherTruthOrigin == 38 || motherTruthOrigin == 39)) ||
        (motherTruthType == 16 && motherTruthOrigin == 42))
      return TruthHelpers::SM4tTruthCategoryFlag::MaterialConv;

    if (motherTruthType == 4 && motherTruthOrigin == 5) return TruthHelpers::SM4tTruthCategoryFlag::GamstarConv;
  }
  if (truthOrigin == 23 || truthOrigin == 24 || truthOrigin == 30 || truthOrigin == 31)
    return TruthHelpers::SM4tTruthCategoryFlag::Fake;
  if (truthOrigin == 25 || truthOrigin == 32) return TruthHelpers::SM4tTruthCategoryFlag::HFc;
  if (truthOrigin == 26 || truthOrigin == 33) return TruthHelpers::SM4tTruthCategoryFlag::HFb;

  return TruthHelpers::SM4tTruthCategoryFlag::Other;
}

TruthHelpers::SM4tTruthCategoryFlag TruthHelpers::muonBkgFlagSM4t(std::shared_ptr<TTHbb::Lepton> mu) {
  int truthType = mu->intVariable("mu_true_type");
  int truthOrigin = mu->intVariable("mu_true_origin");

  if ((truthType >= 6 && truthType <= 8) && ((truthOrigin >= 9 && truthOrigin <= 14) || truthOrigin == 43))
    return TruthHelpers::SM4tTruthCategoryFlag::Prompt;
  if ((truthType == 0 || truthType == 7 || truthType == 8) && truthOrigin == 0)
    return TruthHelpers::SM4tTruthCategoryFlag::GamstarConv;
  if (truthType == 7 || truthType == 8) {
    if (truthOrigin == 25 || truthOrigin == 32) return TruthHelpers::SM4tTruthCategoryFlag::HFc;
    else if (truthOrigin == 26 || truthOrigin == 33) return TruthHelpers::SM4tTruthCategoryFlag::HFb;
  }
  return TruthHelpers::SM4tTruthCategoryFlag::Other;
}

TruthHelpers::SM4tTruthCategoryFlag TruthHelpers::leptonBkgFlagSM4t(std::shared_ptr<TTHbb::Lepton> lep) {
  // see: https://gitlab.cern.ch/atlasphys-top/xs/4tops/frameworks/common-framework/issues/89#note_2335911
  switch (lep->type()) {
    case TTHbb::leptonType::electron:
      return electronBkgFlagSM4t(lep);
      break;  // The break is for aesthetics
    case TTHbb::leptonType::muon:
      return muonBkgFlagSM4t(lep);
      break;
    default:
      return TruthHelpers::SM4tTruthCategoryFlag::Other;
  }
}

  TruthHelpers::SM4tBkgCategory TruthHelpers::leptonBkgCategorisation(int chan, TruthHelpers::SM4tTruthCategoryFlag lep0, TruthHelpers::SM4tTruthCategoryFlag lep1, TruthHelpers::SM4tTruthCategoryFlag lep2) {
    TruthHelpers::SM4tBkgCategory bkg_cat=TruthHelpers::SM4tBkgCategory::UNDEFINED;
  
    // is it 2L or 3L?
    bool is2L = chan<=3;
    bool is3L = chan>=4;

    // Physics background in 2L
    if (is2L && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt)
      return TruthHelpers::SM4tBkgCategory::prompt;
  
    // QmisID in 2L
    bool lep0_trident = (lep0==TruthHelpers::SM4tTruthCategoryFlag::TridentFlipQ && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep1_trident = (lep1==TruthHelpers::SM4tTruthCategoryFlag::TridentFlipQ && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep0_fQ = (lep0==TruthHelpers::SM4tTruthCategoryFlag::PromptFlipQ && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep1_fQ = (lep1==TruthHelpers::SM4tTruthCategoryFlag::PromptFlipQ && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is2L && ((lep0_trident||lep1_trident) || (lep0_fQ||lep1_fQ)) )
      return TruthHelpers::SM4tBkgCategory::qmID;
  
    // Real photon conversion in 2L
    bool lep0_conv = (lep0==TruthHelpers::SM4tTruthCategoryFlag::MaterialConv && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep1_conv = (lep1==TruthHelpers::SM4tTruthCategoryFlag::MaterialConv && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is2L && (lep0_conv||lep1_conv))
      return TruthHelpers::SM4tBkgCategory::materialconv;
  
    // Virtual photon conversion in 2L
    bool lep0_gstar = (lep0==TruthHelpers::SM4tTruthCategoryFlag::GamstarConv && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep1_gstar = (lep1==TruthHelpers::SM4tTruthCategoryFlag::GamstarConv && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is2L && (lep0_gstar||lep1_gstar))
      return TruthHelpers::SM4tBkgCategory::gamstarconv;
  
    // Heavy flavour decay in 2L
    bool lep0_HF = ((lep0==TruthHelpers::SM4tTruthCategoryFlag::HFb||lep0==TruthHelpers::SM4tTruthCategoryFlag::HFc) && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep1_HF = ((lep1==TruthHelpers::SM4tTruthCategoryFlag::HFb||lep1==TruthHelpers::SM4tTruthCategoryFlag::HFc) && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is2L && (lep0_HF||lep1_HF))
      return TruthHelpers::SM4tBkgCategory::HF;
  
    // Fake leptons in 2L
    bool lep0_FK = (lep0==TruthHelpers::SM4tTruthCategoryFlag::Fake && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep1_FK = (lep1==TruthHelpers::SM4tTruthCategoryFlag::Fake && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is2L && (lep0_FK||lep1_FK))
      return TruthHelpers::SM4tBkgCategory::fake;
  
    // Physics in 3L
    if (is3L && (lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt))
      bkg_cat = TruthHelpers::SM4tBkgCategory::prompt;
  
    // Real photon conversion in 3L
    lep0_conv = (lep0==TruthHelpers::SM4tTruthCategoryFlag::MaterialConv && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    lep1_conv = (lep1==TruthHelpers::SM4tTruthCategoryFlag::MaterialConv && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep2_conv = (lep2==TruthHelpers::SM4tTruthCategoryFlag::MaterialConv && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is3L && (lep0_conv||lep1_conv||lep2_conv))
      return TruthHelpers::SM4tBkgCategory::materialconv;
  
    // Virtual photon conversion in 3L
    lep0_gstar = (lep0==TruthHelpers::SM4tTruthCategoryFlag::GamstarConv && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    lep1_gstar = (lep1==TruthHelpers::SM4tTruthCategoryFlag::GamstarConv && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep2_gstar = (lep2==TruthHelpers::SM4tTruthCategoryFlag::GamstarConv && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is3L && (lep0_gstar||lep1_gstar||lep2_gstar))
      return TruthHelpers::SM4tBkgCategory::gamstarconv;
  
    // Heavy flavour decay in 3L
    lep0_HF = ((lep0==TruthHelpers::SM4tTruthCategoryFlag::HFb||lep0==TruthHelpers::SM4tTruthCategoryFlag::HFc) && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    lep1_HF = ((lep1==TruthHelpers::SM4tTruthCategoryFlag::HFb||lep1==TruthHelpers::SM4tTruthCategoryFlag::HFc) && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep2_HF = ((lep2==TruthHelpers::SM4tTruthCategoryFlag::HFb||lep2==TruthHelpers::SM4tTruthCategoryFlag::HFc) && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is3L && (lep0_HF||lep1_HF||lep2_HF))
      return TruthHelpers::SM4tBkgCategory::HF;
  
    // Fake leptons in 3L
    lep0_FK = (lep0==TruthHelpers::SM4tTruthCategoryFlag::Fake && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    lep1_FK = (lep1==TruthHelpers::SM4tTruthCategoryFlag::Fake && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep2==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    bool lep2_FK = (lep2==TruthHelpers::SM4tTruthCategoryFlag::Fake && lep0==TruthHelpers::SM4tTruthCategoryFlag::Prompt && lep1==TruthHelpers::SM4tTruthCategoryFlag::Prompt);
    if (is3L && (lep0_FK||lep1_FK||lep2_FK))
      return TruthHelpers::SM4tBkgCategory::fake;
 
    // If none of the above, others
    if (bkg_cat==TruthHelpers::SM4tBkgCategory::UNDEFINED)
      bkg_cat=TruthHelpers::SM4tBkgCategory::other;

    return bkg_cat;

  
}

}  // namespace SM4t
