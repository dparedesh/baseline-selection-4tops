#include "SM4topsSSML/ZVetoSelector.h"
#include "TopEvent/EventTools.h"

#include "TTHbbObjects/Event.h"


namespace SM4topsSSML {
ZVetoSelector::ZVetoSelector(std::string& params) :
  m_cutvalue0(81000.),
  m_cutvalue1(101000.),
  m_useLooseLeptons(false),
  m_is_in_window(false)
{
  std::istringstream iss(params);
  iss >> m_cutvalue0 >> m_cutvalue1;

  bool use_loose_leptons;
  if (iss >> use_loose_leptons) m_useLooseLeptons = use_loose_leptons;

  bool is_in_window;
  if (iss >> is_in_window) m_is_in_window = is_in_window;
}

bool ZVetoSelector::apply(const top::Event& event) const
{
  std::shared_ptr<TTHbb::Event> tthevt = 
    event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  unsigned int nleptons = tthevt->m_leptons.size();
  if (nleptons < 2) return false;

  std::vector<TLorentzVector> electrons_sig, muons_sig;
  std::vector<double> electrons_sig_charge, muons_sig_charge;

  // loop over the 3 first loose leptons
  unsigned int index_lep_max = (nleptons < 3) ? nleptons : 3;
  for (unsigned int index_lep = 0; index_lep < index_lep_max; index_lep++){
    
    // if use only tight events, ignore non-tight leptons
    if (!m_useLooseLeptons && !tthevt->m_leptons.at(index_lep)->charVariable("isTight") ) continue;

    // if electron
    if ( tthevt->m_leptons.at(index_lep)->type() == TTHbb::leptonType::electron ){
      electrons_sig.push_back(  tthevt->m_leptons.at(index_lep)->p4()  );
      electrons_sig_charge.push_back( tthevt->m_leptons.at(index_lep)->floatVariable("charge")  );
    }

    // if muon
    else {
      muons_sig.push_back(  tthevt->m_leptons.at(index_lep)->p4()  );
      muons_sig_charge.push_back( tthevt->m_leptons.at(index_lep)->floatVariable("charge")  );
    }     
  }

  bool pairInWindow = false;
  unsigned int index_el_max = electrons_sig.size();
  unsigned int index_mu_max = muons_sig.size();

  // loop over electron pairs
  if (index_el_max > 1) {
    for (unsigned int i = 0; i < index_el_max-1 ; i++) {
      for (unsigned int j = i+1; j < index_el_max; j++) {
	if (electrons_sig_charge[i]*electrons_sig_charge[j] == -1.) {
	  const double mll = (electrons_sig[i]+electrons_sig[j]).M() ; // top::invariantMass(*electrons_sig[i], *electrons_sig[j]);
	  if (mll >= m_cutvalue0 && mll <= m_cutvalue1) pairInWindow = true;
	}
      }
    }
  }

  // loop over muon pairs
  if (index_mu_max > 1) {
    for (unsigned int i = 0; i < index_mu_max-1 ; i++) {
      for (unsigned int j = i+1; j < index_mu_max; j++) {
	if (muons_sig_charge[i]*muons_sig_charge[j] == -1.) {
	  const double mll = (muons_sig[i]+muons_sig[j]).M(); // top::invariantMass(*muons_sig[i], *muons_sig[j]);
	  if (mll >= m_cutvalue0 && mll <= m_cutvalue1) pairInWindow = true;
	}
      }
    }
  }

  if (!m_is_in_window && !pairInWindow) return true;
  else if (m_is_in_window && pairInWindow) return true;
  else return false;
}

std::string ZVetoSelector::name() const {
  std::stringstream ss;
  ss << "SM4TOPSZVETO " << m_cutvalue0 << " " << m_cutvalue1
     << " " << m_useLooseLeptons << " " << m_is_in_window;
  return ss.str();
}
}
