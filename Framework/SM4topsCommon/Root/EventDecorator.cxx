#include "SM4topsCommon/EventDecorator.h"

#include <sstream>
#include <algorithm>
#include <numeric>
#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbObjects/Event.h"

namespace SM4topsCommon {

EventDecorator::EventDecorator(std::string params,std::shared_ptr<top::TopConfig> config)  {

  // Event decoration:
  auto* ehs = EventSaverHelper::get();

  // Add Event truth 
  ehs->addVariableToBeSaved("HT_all", TTHbb::DecorationType::Float, false, "EventDecorator");
  ehs->addVariableToBeSaved("HT_jets", TTHbb::DecorationType::Float, false, "EventDecorator");
  ehs->addVariableToBeSaved("nRCJets", TTHbb::DecorationType::Int, false, "EventDecorator");
  ehs->addVariableToBeSaved("nRCJetsM100", TTHbb::DecorationType::Int, false, "EventDecorator");
  ehs->addVariableToBeSaved("mJSum", TTHbb::DecorationType::Float, false, "EventDecorator");
  
  // isTruthMatched flag
  m_isMC = config->isMC(); 
  ehs->addVariableToBeSaved("isTruthMatched", TTHbb::DecorationType::Char, false, "EventDecorator");

}

bool EventDecorator::apply(const top::Event & event) const{
  
  // Check if TTHbb::Event exists
  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout << "EventDecorator: TTHbbEventVariables (TTHbb::Event*) object not found" 
	      << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  // Extract TTHbb::Event
  std::shared_ptr<TTHbb::Event> tthevt = 
    event.m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

     
  // Event decoration:
    
  if(!tthevt->floatVariable("HT_all")) { // if variables not already filled
    tthevt->floatVariable("HT_all") = top::ht(event);
    tthevt->floatVariable("HT_jets") = std::accumulate(event.m_jets.begin(), event.m_jets.end(), 0., [](float init, const xAOD::Jet *jet) {return init + jet->pt();});
    decorateRCJets(tthevt);
  }

  //isTruthMatch flag
  //// The flag is True in case all the leptons are matched, is False in case at least one lepton is not matched
  //// Only the first three pT-ordered loose leptons are checked (SSML need), that fits also for 1LOS channel
  bool isTruthMatched = true;
  if (m_isMC){
    int nLeptons = (tthevt->m_leptons.size() < 3) ? tthevt->m_leptons.size() : 3;
    for (int ilep = 0; ilep < nLeptons; ilep++)
      {
	if (!tthevt->m_leptons.at(ilep)->charVariable("isTight")) continue;
  	isTruthMatched &= (bool)(tthevt->m_leptons.at(ilep)->charVariable("isPrompt"));
      }
  }
  tthevt->charVariable("isTruthMatched") = isTruthMatched;
  
  return true;
}

std::string EventDecorator::name() const {
  return "SM4topsCommonEventDecorator";
}

void EventDecorator::decorateRCJets(std::shared_ptr<TTHbb::Event> tthevt) const {

  // Extract the RC jets
  auto rcjets = tthevt->m_customObj["rcjet"];

  int nRCJetsM100=0, nRCJets=0;
  float mJSum=0;

  for (auto &rcjet: rcjets){      
    if( rcjet->Pt() < 200000 || fabs(rcjet->Eta()) > 2.0 ) continue;
    if( rcjet->M()>100000 ) nRCJetsM100++;
    if( nRCJets<4 ) mJSum += rcjet->M();
    nRCJets++;
  }
    
  tthevt->intVariable("nRCJetsM100") = nRCJetsM100;
  tthevt->intVariable("nRCJets") = nRCJets;
  tthevt->floatVariable("mJSum") = mJSum;
}

} // End of SM4topsCommon
