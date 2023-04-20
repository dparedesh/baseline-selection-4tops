/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"
#include "TTHbbLeptonic/TruthHistoryHelper.h"

#ifndef TruthDecorator_H_
#define TruthDecorator_H_

namespace SM4topsCommon {
class TruthDecorator:public top::EventSelectorBase {
  public:

    TruthDecorator(std::string params, std::shared_ptr<top::TopConfig> config);
    ~TruthDecorator();

    //    void initialise(const top::Event& event) const;
    bool apply(const top::Event & event) const override;
    std::string name() const override;

  private:
    bool m_debug;

    TTHbbAsgToolHelper* m_asgHelper;

    //event
    mutable const top::Event* m_event;
    std::shared_ptr<top::TopConfig> m_config;

    std::string m_params;

    //functions
    const xAOD::Jet* FindxAODJets(std::shared_ptr<TTHbb::Jet>& jet,
				  const xAOD::JetContainer& xAODJets) const;
    const xAOD::TruthParticle* getParent(const xAOD::TruthParticle* part) const;
    const xAOD::TruthParticle* getParentTop(const xAOD::TruthParticle* part) const;

    void DecorateJetsWithGhost() const;
    void DecorateTruthHt() const;

  };
}

#endif
