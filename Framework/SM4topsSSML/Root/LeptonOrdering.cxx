#include "SM4topsSSML/LeptonOrdering.h"

#include "TTHbbObjects/Event.h"
#include "MVAVariables/JetOrderingTool.h"

namespace SM4topsSSML {

LeptonOrdering::LeptonOrdering(const std::string& params, std::shared_ptr<top::TopConfig> config) {}

bool LeptonOrdering::apply(const top::Event& event) const
{
  std::shared_ptr<TTHbb::Event> tthevt = 
    event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  // Use the JetOrderingTool from TTHbb to order the lepton in decreasing pt 
  JetOrderingTool::sortPt(tthevt->m_leptons);

  return true;
}

std::string  LeptonOrdering::name() const {  
  return "LEPTONORDERING";;
}

}
