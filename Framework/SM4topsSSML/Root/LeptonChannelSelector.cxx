#include "SM4topsSSML/LeptonChannelSelector.h"
#include <cmath>

#include "TTHbbObjects/Event.h"
#include <AsgTools/MessageCheck.h>

using namespace asg::msgUserCode;


namespace SM4topsSSML {

LeptonChannelSelector::LeptonChannelSelector(const std::string& params, std::shared_ptr<top::TopConfig> config) :
  m_channel(UNDEFINED),
  m_use_loose_leptons(false)
{
  std::istringstream iss(params);

  iss >> m_channel;
  if (m_channel < UNDEFINED || m_channel >= SM4topsSSMLChannelMAX) m_channel=UNDEFINED;

  bool use_loose_leptons;
  if (iss >> use_loose_leptons) m_use_loose_leptons = use_loose_leptons;
}

bool LeptonChannelSelector::apply(const top::Event& event) const
{
  std::shared_ptr<TTHbb::Event> tthevt =
    event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  unsigned int nleptons = tthevt->m_leptons.size();
  if (nleptons < 2) return false;

  int tight_lep_n_in_lll(0);
  int tight_el_n_in_lll(0);
  int tight_centralel_n_in_lll(0);
  int tight_mu_n_in_lll(0);

  // loop over the 3 first loose leptons
  unsigned int index_lep_max = (nleptons < 3) ? nleptons : 3;
  for (unsigned int index_lep = 0; index_lep < index_lep_max; index_lep++){

    // if use only tight events, ignore non-tight leptons
    if (!m_use_loose_leptons && !tthevt->m_leptons.at(index_lep)->charVariable("isTight") ) continue;

    if ( tthevt->m_leptons.at(index_lep)->type() == TTHbb::leptonType::electron ){
      ++tight_lep_n_in_lll;
      ++tight_el_n_in_lll;
      // if (std::abs(event.m_electrons.at(lep.second.second)->eta()) < 1.37) ++tight_centralel_n_in_lll;
      ++tight_centralel_n_in_lll;
    }

    else {
      ++tight_lep_n_in_lll;
      ++tight_mu_n_in_lll;
    }
  }

  // Lepton channel categorization
  switch ( SM4topsSSMLChannel(m_channel) )
    {
    case EE:
      return tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==2;
      break;
    case MM:
      return tight_lep_n_in_lll==2 && tight_el_n_in_lll==0;
      break;
    case EM:
      return tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==1 && tight_mu_n_in_lll==1;
      break;
    case DILEP:
      return (tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==2) || (tight_lep_n_in_lll==2 && tight_el_n_in_lll==0) || (tight_lep_n_in_lll == 2 && tight_centralel_n_in_lll==1 && tight_mu_n_in_lll==1);
      break;
    case EEE:
      return tight_lep_n_in_lll==3 && tight_el_n_in_lll==3;
      break;
    case EEM:
      return tight_lep_n_in_lll==3 && tight_el_n_in_lll==2;
      break;
    case EMM:
      return tight_lep_n_in_lll==3 && tight_el_n_in_lll==1;
      break;
    case MMM:
      return tight_lep_n_in_lll==3 && tight_el_n_in_lll==0;
      break;
    case TRILEP:
      return tight_lep_n_in_lll==3;
      break;
    case DILEPORTRILEP:
      return tight_lep_n_in_lll==2 || tight_lep_n_in_lll==3;
      break;
    default:
      std::cout << "WARNING: Unknown channel passed to LeptonChannelSelector" << std::endl;
    }

  return false;
}

std::string  LeptonChannelSelector::name() const {
  std::stringstream ss;
  ss << "SM4TOPSSSMLCHANNEL == " << m_channel << " " << m_use_loose_leptons;
  return ss.str();
}

}
