#include "SM4topsCommon/ElectronDecorator.h"
#include "SM4topsCommon/ToolBox.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include "TopEvent/EventTools.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include <AsgTools/MessageCheck.h>
#include <AthContainers/exceptions.h>

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbObjects/Event.h"

using namespace asg::msgUserCode;

namespace SM4topsCommon {

ElectronDecorator::ElectronDecorator(const std::string &selectorName, const std::string &params,
                                     std::shared_ptr<top::TopConfig> config) {
  auto *esh = EventSaverHelper::get();
  auto *toolBox = ToolBox::get();

  m_electronTriggerNames =
    toolBox->getElectronTriggerNames(toolBox->getUniqueTriggerNamesAll(config, selectorName));
  m_electronSLTriggerNames = toolBox->getSLTriggerNames(m_electronTriggerNames);
  m_electronDLTriggerNames = toolBox->getDLTriggerNames(m_electronTriggerNames);

  std::cout << "Decorating electrons for selector: " << selectorName << std::endl;
  std::cout << "TriggerMatch variables for: " << std::endl;
  for (auto &triggerName : m_electronTriggerNames) {
    std::cout << "-- " << triggerName << std::endl;
    /* At the moment, TTHbb already creates this variable with a lower case T (i.e. trigMatch_), but
     * their trigger list is hardcoded.*/
    // esh->addElecVariableToBeSaved(std::string("TrigMatch_") + triggerName,
    //                               TTHbb::DecorationType::Char, false, "ElectronDecorator");
  }

  std::cout << "Of those are single lepton:" << std::endl;
  for_each(m_electronSLTriggerNames.begin(), m_electronSLTriggerNames.end(),
           [](string &s) { std::cout << "- " << s << std::endl; });
  std::cout << "And dilepton:" << std::endl;
  for_each(m_electronDLTriggerNames.begin(), m_electronDLTriggerNames.end(),
           [](string &s) { std::cout << "- " << s << std::endl; });

  esh->addElecVariableToBeSaved("isSLTMatch", TTHbb::DecorationType::Char, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("isDLTMatch", TTHbb::DecorationType::Char, false,
                                "ElectronDecorator");
  //truth related variable

  // checking isMC
  m_isMC = config->isMC();

  //add prompt tag
  esh->addElecVariableToBeSaved("isPrompt", TTHbb::DecorationType::Char, false,
                                "ElectronDecorator");
  //Add the motherId variable
  esh->addElecVariableToBeSaved("truthPdgId", TTHbb::DecorationType::Int, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("firstEgMotherPdgId", TTHbb::DecorationType::Int, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("firstEgMotherTruthType", TTHbb::DecorationType::Int, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("firstEgMotherTruthOrigin", TTHbb::DecorationType::Int, false,
                                "ElectronDecorator");

}

bool ElectronDecorator::apply(const top::Event &event) const {
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

  // Create all the necessary accessors for all triggers.
  std::map<std::string, SG::AuxElement::ConstAccessor<char> *> accessors = {};
  for (auto &triggerName : m_electronTriggerNames) {
    auto acc = new SG::AuxElement::ConstAccessor<char>(std::string("TRIGMATCH_") + triggerName);
    accessors[triggerName] = acc;
  }

  /* Loop over electrons. Use an index loop to be able to access the TTHbb electron as well as the
   * xAODElectron */
  for (unsigned int iel = 0; iel < tthevt->m_electrons.size(); iel++) {
    // for (auto &triggerName : m_electronTriggerNames) {
    //   auto acc = *accessors[triggerName];
    //   tthevt->m_electrons.at(iel)->charVariable(std::string("TrigMatch_") + triggerName) =
    //       acc.isAvailable(*event.m_electrons.at(iel)) ? acc(*event.m_electrons.at(iel)) : false;
    //   /* This means that if the Aux store is not available it still runs through at sets false as
    //    * default. This might not be the desired behaviour. */
    // }

    for (auto &triggerName : m_electronSLTriggerNames) {
      auto acc = *accessors[triggerName];
      if (acc.isAvailable(*event.m_electrons.at(iel))) {
        tthevt->m_electrons.at(iel)->charVariable("isSLTMatch") = acc(*event.m_electrons.at(iel));
        if (acc(*event.m_electrons.at(iel))) break;
      } else {
        tthevt->m_electrons.at(iel)->charVariable("isSLTMatch") = false;
      }
    }
    for (auto &triggerName : m_electronDLTriggerNames) {
      auto acc = *accessors[triggerName];
      if (acc.isAvailable(*event.m_electrons.at(iel))) {
        tthevt->m_electrons.at(iel)->charVariable("isDLTMatch") = acc(*event.m_electrons.at(iel));
        if (acc(*event.m_electrons.at(iel))) break;
      } else {
        tthevt->m_electrons.at(iel)->charVariable("isDLTMatch") = false;
      }
    }

    //Truth PDG ID
    int truthPdgId = -999;
    if (m_isMC) truthPdgId = event.m_electrons.at(iel)->auxdata<int>("truthPdgId");
    tthevt->m_electrons.at(iel)->intVariable("truthPdgId") = truthPdgId;

    //Prompt tagging
    //1.isolated electron with charge correctly measured
    //2.bremstra. electron with electron truth charge equal to parent truth charge
    bool el_isPrompt = false;
    if (m_isMC){
      if (event.m_electrons.at(iel)->auxdataConst< int >("truthType") == 2
	  && (event.m_electrons.at(iel)->charge()*event.m_electrons.at(iel)->auxdataConst< int >("truthPdgId")) < 0)
	el_isPrompt = true;

      if (event.m_electrons.at(iel)->auxdataConst< int >("truthType") == 4
	  && event.m_electrons.at(iel)->auxdata<int>("firstEgMotherTruthType") == 2
	  && (event.m_electrons.at(iel)->charge()*event.m_electrons.at(iel)->auxdataConst< int >("firstEgMotherPdgId")) < 0)
	el_isPrompt = true;
    } // isMC
    tthevt->m_electrons.at(iel)->charVariable("isPrompt") = el_isPrompt;

    //Mother ID variable
    int fEgMothPdgId = -999;
    int fEgMothTruthOrigin = -999;
    int fEgMothTruthType = -999;
    if (m_isMC){
    fEgMothPdgId = event.m_electrons.at(iel)->auxdata<int>("firstEgMotherPdgId");
    fEgMothTruthOrigin = event.m_electrons.at(iel)->auxdata<int>("firstEgMotherTruthOrigin");
    fEgMothTruthType = event.m_electrons.at(iel)->auxdata<int>("firstEgMotherTruthType");
    } // isMC
    tthevt->m_electrons.at(iel)->intVariable("firstEgMotherPdgId") = fEgMothPdgId ;
    tthevt->m_electrons.at(iel)->intVariable("firstEgMotherTruthOrigin") = fEgMothTruthOrigin;
    tthevt->m_electrons.at(iel)->intVariable("firstEgMotherTruthType") = fEgMothTruthType;

  }  // End electron loop

  // free allocated memory
  std::map<std::string, SG::AuxElement::ConstAccessor<char> *>::iterator itr = accessors.begin();
  while(itr != accessors.end())
    {
      delete itr->second;
      itr++;
    }
 
  return true;
}

std::string ElectronDecorator::name() const { return "DECORATEELECTRONS"; }

// bool ElectronDecorator::isSLTMatch(const std::map<std::string,
// SG::AuxElement::ConstAccessor<char> *> &accessors, const xAOD::Electron &el) const{
//   for (auto trigNameAccessorPair : accessors) {
//     std::smatch match;
//     while(std::regex_search (trigNameAccessorPair.first, match, std::regex("(e|mu)[0-9]{1,3}")))
//     {
//       // for_each(match.begin(),match.end(), [] (std::string s) {std::cout << s << std::endl;});
//       std::cout << match[0] << std::endl;
//     }
//   }
//   return false;
// }

}  // namespace SM4topsCommon
