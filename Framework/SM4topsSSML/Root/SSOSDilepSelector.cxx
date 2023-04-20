#include "SM4topsSSML/SSOSDilepSelector.h"
#include "TopConfiguration/TopConfig.h"

#include "TTHbbObjects/Event.h"

namespace SM4topsSSML {

SSOSDilepSelector::SSOSDilepSelector(const std::string& params, std::shared_ptr<top::TopConfig> config) :
  m_isOS(false),
  m_useLooseLeptons(false)
{
  std::istringstream iss(params);

  bool isOS;
  if (iss >> isOS) m_isOS = isOS;

  bool useLooseLeptons;
  if (iss >> useLooseLeptons) m_useLooseLeptons = useLooseLeptons;

}

bool SSOSDilepSelector::apply(const top::Event& event) const
{
  std::shared_ptr<TTHbb::Event> tthevt = 
    event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  unsigned int nleptons = tthevt->m_leptons.size();
  if (nleptons < 2) return false;

  std::vector<double> leptons_sig;

  // loop over the 3 first loose leptons
  unsigned int index_lep_max = (nleptons < 3) ? nleptons : 3;
  for (unsigned int index_lep = 0; index_lep < index_lep_max; index_lep++){
    
    // if use only tight events, ignore non-tight leptons
    if (!m_useLooseLeptons && !tthevt->m_leptons.at(index_lep)->charVariable("isTight") ) continue;

    leptons_sig.push_back( tthevt->m_leptons.at(index_lep)->floatVariable("charge") );
  }

  if (leptons_sig.size() != 2) return false;

  return m_isOS ? leptons_sig[0]!=leptons_sig[1] : leptons_sig[0]==leptons_sig[1];
}

std::string SSOSDilepSelector::name() const {
  std::stringstream ss;
  ss << "SSOSDilepSelector " << m_useLooseLeptons << " " << m_isOS;
  return ss.str();
}
}
