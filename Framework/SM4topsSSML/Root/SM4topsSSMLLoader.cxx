/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SM4topsSSML/SM4topsSSMLLoader.h"
#include "SM4topsSSML/LeptonOrdering.h"
#include "SM4topsSSML/LeptonChannelSelector.h"
#include "SM4topsSSML/TrigMatch.h"
#include "SM4topsSSML/EventDecorator.h"
#include "SM4topsSSML/TruthMatchSelector.h"
#include "SM4topsSSML/MllSelector.h"
#include "SM4topsSSML/MllWindowSelector.h"
#include "SM4topsSSML/SSOSDilepSelector.h"
#include "SM4topsSSML/ZVetoSelector.h"
#include "SM4topsSSML/LeptonVectorDecorator.h"
#include "SM4topsSSML/ElectronDecorator.h"

#include <string>
#include <sstream>
#include <iostream>

/**
  * When top-xaod recognises a cut specification in the configuration file, it
  * attempts to load the corresponding cut tool.
  */

top::EventSelectorBase* SM4topsSSMLLoader::initTool(
  const std::string& name,
  const std::string& line,
  TFile* /*outputFile*/,
  std::shared_ptr<top::TopConfig> config,
  EL::Worker* /*wk*/
){
  std::istringstream iss(line);
  std::string toolname;
  getline(iss, toolname, ' ');

  std::string param;
  if (line.size() > toolname.size()) param = line.substr(toolname.size() + 1);

  if (toolname == "LEPTONORDERING")
    return new SM4topsSSML::LeptonOrdering(param, config);

  if (toolname == "SM4TOPSSSMLCHANNEL")
    return new SM4topsSSML::LeptonChannelSelector(param, config);

  if (toolname == "SM4TOPSSSMLTRIGMATCH")
    return new SM4topsSSML::TrigMatch(name, config);

  if (toolname == "SM4TOPSSSMLEVENTDECORATOR")
    return new SM4topsSSML::EventDecorator(param, config);

  if (toolname == "SM4TOPSSSMLTRUTHMATCH")
    return new SM4topsSSML::TruthMatchSelector(param, config);

  if (toolname == "SM4TOPSSSMLMLL")
    return new SM4topsSSML::MllSelector(param, false);

  if (toolname == "SM4TOPSSSMLMLLLOOSE")
    return new SM4topsSSML::MllSelector(param, true);


  if (toolname == "SM4TOPSSSMLMLLWIN")
    return new SM4topsSSML::MllWindowSelector(param);

  if (toolname == "SM4TOPSSSMLSSOSDILEPSELECTOR")
    return new SM4topsSSML::SSOSDilepSelector(param, config);

  if (toolname == "SM4TOPSSMLZVETO")
    return new SM4topsSSML::ZVetoSelector(param);

  if (toolname == "LEPTONVARS")
    return new SM4topsSSML::LeptonVectorDecorator(param, config);

  if (toolname == "DECORATEELECTRONSSSML")
    return new SM4topsSSML::ElectronDecorator(param, config);

  return nullptr;
}
