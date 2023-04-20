#include "SM4topsSSML/MllSelector.h"
#include "TopEvent/EventTools.h"

#include "TTHbbObjects/Event.h"

namespace SM4topsSSML {
MllSelector::MllSelector(const std::string& params, const bool& useLooseLeptons) :
  SignValueSelector(useLooseLeptons ? "MLL_USELOOSELEPTONS" : "MLL", params),
  m_useLooseLeptons(useLooseLeptons) {}

bool MllSelector::apply(const top::Event& event) const
{
  std::shared_ptr<TTHbb::Event> tthevt = 
    event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  unsigned int nleptons = tthevt->m_leptons.size();
  if (nleptons < 2) return false;

  std::vector<TLorentzVector> leptons_sig;

  // loop over the 3 first loose leptons
  unsigned int index_lep_max = (nleptons < 3) ? nleptons : 3;
  for (unsigned int index_lep = 0; index_lep < index_lep_max; index_lep++){
    
    // if use only tight events, ignore non-tight leptons
    if (!m_useLooseLeptons && !tthevt->m_leptons.at(index_lep)->charVariable("isTight") ) continue;

    leptons_sig.push_back( tthevt->m_leptons.at(index_lep)->p4() );
  }

  if (leptons_sig.size() != 2) return false;

  const double mll = (leptons_sig[0]+leptons_sig[1]).M(); 
  return checkFloat(mll, value()); // https://gitlab.cern.ch/atlas/athena/blob/master/PhysicsAnalysis/TopPhys/xAOD/TopEventSelectionTools/TopEventSelectionTools/SignValueSelector.h#L131
}

std::string MllSelector::name() const {
  std::stringstream ss;
  ss << "SM4topsSSMLMllSelector " << m_useLooseLeptons;
  return ss.str();
}
}
