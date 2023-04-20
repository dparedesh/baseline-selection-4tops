#include "SM4topsSSML/EventDecorator.h"

#include <sstream>
#include <algorithm>

#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbObjects/Event.h"

namespace SM4topsSSML {

  EventDecorator::EventDecorator(std::string params,std::shared_ptr<top::TopConfig> config){


    m_isMC = config->isMC();

    auto *esh = EventSaverHelper::get();

    esh->addVariableToBeSaved("hasMEphoton", TTHbb::DecorationType::Char, false,
			      "EventDecorator");
    esh->addVariableToBeSaved("hasMEphoton_DRgt02_nonhad", TTHbb::DecorationType::Char, false,
			      "EventDecorator");
 
    
}

bool EventDecorator::apply(const top::Event & event) const{

  // Check if TTHbb::Event exists
  if (!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables")) {
    std::cout << "ElectronDecorator: TTHbbEventVariables (TTHbb::Event*) "
      "object not found"
              << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }
  
  // Extract TTHbb::Event
  std::shared_ptr<TTHbb::Event> tthevt =
      event.m_info->auxdecor<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables");
    
  bool hasMEphoton = false;
  bool hasMEphoton_DRgt02_nonhad = false;
  
  if(m_isMC && event.m_truth!=nullptr){
    for (const auto& particle : *(event.m_truth)) {
      int photon_pdgId = 22; // look at photons
      if (particle==nullptr) continue;
      if (particle->parent(0)==nullptr) continue;
      if (fabs(particle->pdgId()) != photon_pdgId) continue;
      if (particle->nParents()==0) continue;
      if (fabs(particle->parent(0)->pdgId()) != photon_pdgId) continue;
      int motherPdgId = 999;
      float motherPt = -1;
      const xAOD::TruthParticle* mother = particle->parent(0); // i am interested in DR only of the mother
      motherPdgId = mother->pdgId();
      motherPt = mother->pt();
      if (abs(motherPdgId) < 100 && particle->barcode() < 2e5 && particle->pt() > 0 && motherPt > 0) {
	hasMEphoton = true;
	if (particle->pt() > 15e3 && (abs(motherPdgId) == 24 || abs(motherPdgId) == 6))
	  hasMEphoton_DRgt02_nonhad = true;
      } // ME photon
    }
  }// isMC and has truth info
  tthevt->charVariable("hasMEphoton")  = hasMEphoton;
  tthevt->charVariable("hasMEphoton_DRgt02_nonhad")  = hasMEphoton_DRgt02_nonhad;


  return true;
}

std::string EventDecorator::name() const{
  return "SM4topsSSMLEventDecorator";
}

}
