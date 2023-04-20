/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Decorates jets with info related to q/g discrimination, such as ...

  -- Track-based variables
      -- track multiplicity
      -- track width
  -- Cluster-based variables
      -- cluster multiplicity
      -- cluster width
      -- ISD multiplicity

  Cluster-based observables are also provided following the application
  of Constituent Subtraction (1403.3108, ATL-PHYS-PUB-2017-020, ATL-PHYS-PUB-2018-011)
*/

#include <cmath>
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <cstdlib>

#include "SM4topsCommon/QGVar.h"

#include "TopEvent/EventTools.h"

#include "xAODTracking/VertexContainer.h"

namespace SM4topsCommon {

//______________________________________________________________________________
QGVar::QGVar(std::string params, std::shared_ptr<top::TopConfig> config):
  m_event(0),
  m_config(config)
{
  m_params=params;
  m_debug = false;

  if ( asg::ToolStore::contains<TTHbbAsgToolHelper>("TTHbbAsgHelper") )
    {
      m_asgHelper = asg::ToolStore::get<TTHbbAsgToolHelper>("TTHbbAsgHelper");
    }
  else
    {
      m_asgHelper = new TTHbbAsgToolHelper("TTHbbAsgHelper");
      top::check( m_asgHelper->initialize() , "Failed to initialize TTHbbAsgHelper" );
    }

  /// register the event decorations that you want to dump
  auto* esh = EventSaverHelper::get();

  // esh->addVariableToBeSaved(name, type, nominalOnly, origin)

  // Cluster-based info
  esh->addJetVariableToBeSaved("N90Constituents", TTHbb::DecorationType::Float, true, "QGVar");
  esh->addJetVariableToBeSaved("Width", TTHbb::DecorationType::Float, true, "QGVar");
  esh->addJetVariableToBeSaved("WidthPhi", TTHbb::DecorationType::Float, true, "QGVar");

  // Track-based info
  esh->addJetVariableToBeSaved("Charge", TTHbb::DecorationType::Float, true, "QGVar");
  esh->addJetVariableToBeSaved("nTrkPt500", TTHbb::DecorationType::Int, true, "QGVar");
  esh->addJetVariableToBeSaved("nTrkPt1000", TTHbb::DecorationType::Int, true, "QGVar");
  esh->addJetVariableToBeSaved("TrackWidthPt500", TTHbb::DecorationType::Float, true, "QGVar");
  esh->addJetVariableToBeSaved("TrackWidthPt1000", TTHbb::DecorationType::Float, true, "QGVar");

  // Quark-gluon tagger variables
  esh->addJetVariableToBeSaved("qg_NTracks", TTHbb::DecorationType::Int, false, "QGVar");
  esh->addJetVariableToBeSaved("qg_TracksWidth", TTHbb::DecorationType::Float, false, "QGVar");
  esh->addJetVariableToBeSaved("qg_TracksC1", TTHbb::DecorationType::Float, false, "QGVar");
  esh->addJetVariableToBeSaved("qg_truth_nCharged", TTHbb::DecorationType::Int, false, "QGVar");
  esh->addJetVariableToBeSaved("qg_truth_pt", TTHbb::DecorationType::Float, false, "QGVar");
  esh->addJetVariableToBeSaved("qg_truth_eta", TTHbb::DecorationType::Float, false, "QGVar");
  esh->addJetVariableToBeSaved("qg_BDT", TTHbb::DecorationType::Float, false, "QGVar");
  esh->addJetVariableToBeSaved("isQuark_WPq50", TTHbb::DecorationType::Int, false, "QGVar");
  esh->addJetVariableToBeSaved("isQuark_WPg50", TTHbb::DecorationType::Int, false, "QGVar");
  esh->addJetVariableToBeSaved("isQuark_WPq80", TTHbb::DecorationType::Int, false, "QGVar");
  esh->addJetVariableToBeSaved("isQuark_WPg80", TTHbb::DecorationType::Int, false, "QGVar");
  esh->addJetVariableToBeSaved("isQuark_WPq90", TTHbb::DecorationType::Int, false, "QGVar");
  esh->addJetVariableToBeSaved("isQuark_WPg90", TTHbb::DecorationType::Int, false, "QGVar");

  if (asg::ToolStore::contains<CP::JetQGTaggerBDT>("myJetQGTaggerBDT")) {
    m_jetQGTaggerBDT = asg::ToolStore::get<CP::JetQGTaggerBDT>("myJetQGTaggerBDT");
  } else {
    std::unique_ptr<CP::JetQGTaggerBDT> jetQGTaggerBDT = nullptr;
    jetQGTaggerBDT = std::unique_ptr<CP::JetQGTaggerBDT>(new CP::JetQGTaggerBDT("myJetQGTaggerBDT"));
    top::check(jetQGTaggerBDT->setProperty("ConfigFile", "JetQGTaggerBDT/JetQGTaggerBDT90Gluon.dat"),
	       "Failed to find the config file for JetQGTaggerBDT");
    top::check(jetQGTaggerBDT->initialize(), "Failed to initialize the JetQGTagger");
    m_jetQGTaggerBDT = jetQGTaggerBDT.release();
  }    

}


//______________________________________________________________________________
QGVar::~QGVar()
{}


// //______________________________________________________________________________
// void QGVar::initialise(const top::Event& event) const{
// }


//______________________________________________________________________________
bool QGVar::apply(const top::Event & event) const
{
  m_event = &event;

  if(!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables")){
    std::cout << "QGVar: TTHbbEventVariables (TTHbb::Event*) object not found" << std::endl;
    std::cout << "-----> more info: <params: " << m_params
	      << "> <systname: " << m_config->systematicName( event.m_hashValue) << ">" << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
  }

  this->DecorateJetsWithQGVars();

  return true;
}


//______________________________________________________________________________
std::string QGVar::name() const{
  return "QGVar";
}


// Find the associated xAO::Jet based on the TTHbb:Jet pt ______________________
const xAOD::Jet* QGVar::FindxAODJets(std::shared_ptr<TTHbb::Jet>& jet,
				     const xAOD::JetContainer& xAODJets) const
{
  for(auto xjet : xAODJets)
    if( (float)jet->p4().Pt() == xjet->pt() ) return xjet;

  return 0;
}

//______________________________________________________________________________
void QGVar::DecorateJetsWithQGVars() const
{

  //--- Get the TTHbb event pointer
  std::shared_ptr<TTHbb::Event> tthevt =
    m_event->m_info->auxdecor<std::shared_ptr<TTHbb::Event> >("TTHbbEventVariables");

  int ij=0;
  //--- Loop over jets
  for(auto jet : tthevt->m_jets){
    ij++;

    //---- Find the corresponding xAOD jet
    const xAOD::Jet* xjet = FindxAODJets(jet, m_event->m_jets);
    if(!xjet){
      std::cout << "QGVar  WARNING   xAOD Jet NOT FOUND" << std::endl;
      continue;
    }

    //----- Get the clusters associated with each jet
    
    //----- Fill decorations
    // Need to grab the primary vertex idx to fill the track-based ones ...

    int junkInt = -999;
    const int HSVertexIdx = FindHSVertexIdx();

    // Cluster-based

    // n90
    static SG::AuxElement::ConstAccessor< float > n90Constituents("N90Constituents");
    if(n90Constituents.isAvailable( *xjet ) ) {
      float n90 = n90Constituents(*xjet);
      jet->floatVariable("N90Constituents") = n90;
    }

    // Width
    static SG::AuxElement::ConstAccessor< float > width("Width");
    if(width.isAvailable( *xjet ) ) {
      float w = width(*xjet);
      jet->floatVariable("Width") = w;
    }

    // WidthPhi
    static SG::AuxElement::ConstAccessor< float > widthPhi("WidthPhi");
    if(widthPhi.isAvailable( *xjet ) ) {
      float wp = widthPhi(*xjet);
      jet->floatVariable("WidthPhi") = wp;
    }


    // Track-based

    // Charge
    static SG::AuxElement::ConstAccessor< float > charge("Charge");
    if(charge.isAvailable( *xjet ) ) {
      float q = charge(*xjet);
      jet->floatVariable("Charge") = q;
    } else { jet->intVariable("Charge") =  junkInt ; }

    // nTrk500
    static SG::AuxElement::ConstAccessor< std::vector<int> > nTrk500("NumTrkPt500");
    if ( nTrk500.isAvailable( *xjet ) ) {
      std::vector<int> vec_nTrk500 = nTrk500(*xjet);
      jet->intVariable("nTrkPt500") = vec_nTrk500[HSVertexIdx];
    } else { jet->intVariable("nTrkPt500") =  junkInt ; }

    // nTrk1000
    static SG::AuxElement::ConstAccessor< std::vector<int> > nTrk1000("NumTrkPt1000");
    if ( nTrk1000.isAvailable( *xjet ) ) {
      std::vector<int> vec_nTrk1000 = nTrk1000(*xjet);
      jet->intVariable("nTrkPt1000") = vec_nTrk1000[HSVertexIdx];
    } else { jet->intVariable("nTrkPt1000") =  junkInt ; }

    // TrackWidthPt500
    static SG::AuxElement::ConstAccessor< std::vector<float> > wTrk500("TrackWidthPt500");
    if ( wTrk500.isAvailable( *xjet ) ) {
      std::vector<float> vec_wTrk500 = wTrk500(*xjet);
      jet->floatVariable("TrackWidthPt500") = vec_wTrk500[HSVertexIdx];
    } else { jet->floatVariable("TrackWidthPt500") =  junkInt*1. ; }

    // TrackWidthPt1000
    static SG::AuxElement::ConstAccessor< std::vector<float> > wTrk1000("TrackWidthPt1000");
    if ( wTrk1000.isAvailable( *xjet ) ) {
      std::vector<float> vec_wTrk1000 = wTrk1000(*xjet);
      jet->floatVariable("TrackWidthPt1000") = vec_wTrk1000[HSVertexIdx];
    } else { jet->floatVariable("TrackWidthPt1000") =  junkInt*1. ; }


    // Quark-gluon tagger variables

    jet->intVariable("qg_NTracks") = m_event->m_jets.at(ij-1)->auxdata<int>("DFCommonJets_QGTagger_NTracks");
    jet->floatVariable("qg_TracksWidth") = m_event->m_jets.at(ij-1)->auxdata<float>("DFCommonJets_QGTagger_TracksWidth");
    jet->floatVariable("qg_TracksC1") = m_event->m_jets.at(ij-1)->auxdata<float>("DFCommonJets_QGTagger_TracksC1");
    jet->intVariable("qg_truth_nCharged") = m_event->m_jets.at(ij-1)->auxdata<int>("DFCommonJets_QGTagger_truthjet_nCharged");
    jet->floatVariable("qg_truth_pt") = m_event->m_jets.at(ij-1)->auxdata<float>("DFCommonJets_QGTagger_truthjet_pt");
    jet->floatVariable("qg_truth_eta") = m_event->m_jets.at(ij-1)->auxdata<float>("DFCommonJets_QGTagger_truthjet_eta");

    float bdt = m_jetQGTaggerBDT->getScore(*xjet);
    float jet_pt_gev = jet->Pt()/1000;
    jet->floatVariable("qg_BDT") = bdt;
    jet->intVariable("isQuark_WPq50") = bdt > ( (jet_pt_gev<200)*(-0.000714*jet_pt_gev-0.0121) + (jet_pt_gev>=200)*-0.155 );
    jet->intVariable("isQuark_WPg50") = bdt > ( (jet_pt_gev<100)*(0.0012*jet_pt_gev+0.02) + (jet_pt_gev>=100)*0.14 );
    jet->intVariable("isQuark_WPq80") = bdt > 0.05;
    jet->intVariable("isQuark_WPg80") = bdt > -0.02;
    jet->intVariable("isQuark_WPq90") = bdt > 0.14;
    jet->intVariable("isQuark_WPg90") = bdt > -0.11;

  }
}

int QGVar::FindHSVertexIdx() const
{

  // Get the vertices container
  const xAOD::VertexContainer* vertices =  m_event->m_primaryVertices;
  if (vertices->size() == 0 ) {
    //std::cout << "There are no vertices in the container. Exiting"  << std::endl;
    return -999;
  }

  // Find the primary vertex and return its index
  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ){
    if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx){
      //std::cout << name() << " Found HS vertex at index: " << iVertex  << std::endl;
      return iVertex;
    }
  }

  // Else default to 0
  //std::cout << "There is no vertex of type PriVx. Taking default vertex." << std::endl;
  return 0;
}
}
