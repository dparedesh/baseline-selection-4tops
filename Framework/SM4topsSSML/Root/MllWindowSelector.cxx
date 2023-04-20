#include "SM4topsSSML/MllWindowSelector.h"
#include "TopEvent/EventTools.h"

#include "TTHbbObjects/Event.h"

namespace SM4topsSSML {

MllWindowSelector::MllWindowSelector(std::string& params) :
  m_cutvalue0(0.),
  m_cutvalue1(0.),
  m_use_loose_leptons(false),
  m_is_in_window(false)
{
  std::istringstream iss(params);
  iss >> m_cutvalue0 >> m_cutvalue1;

  bool use_loose_leptons;
  if (iss >> use_loose_leptons) m_use_loose_leptons = use_loose_leptons;

  bool is_in_window;
  if (iss >> is_in_window) m_is_in_window = is_in_window;
}

bool MllWindowSelector::apply(const top::Event& event) const
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
    if (!m_use_loose_leptons && !tthevt->m_leptons.at(index_lep)->charVariable("isTight") ) continue;

    leptons_sig.push_back( tthevt->m_leptons.at(index_lep)->p4() );
  }

  if (leptons_sig.size() != 2) return false;

  const double mll = (leptons_sig[0]+leptons_sig[1]).M(); 
  if (m_is_in_window) return mll >= m_cutvalue0 && mll <= m_cutvalue1;
  else return mll < m_cutvalue0 || mll > m_cutvalue1;
}

std::string MllWindowSelector::name() const {
  std::stringstream ss;
  ss << "SM4TOPSSSMLMLLWINDOW " << m_cutvalue0 << " " << m_cutvalue1
     << " " << m_use_loose_leptons << " " << m_is_in_window;
  return ss.str();
}
}
