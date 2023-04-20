#include "OffSM4topsTool/AddBDT1LOS.h"
#include "TTHbbToolManager/ToolManager.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

#include "TTHbbToolManager/OfflineMVAVariables.h"
#include "TTHbbToolManager/ToolManager.h"

#include <TSystem.h>

namespace TTHbb{

  //____________________________________________________
  void AddBDT1LOS::initialise() {

    // Extract configuration
    auto* config = TTHbb::GlobalConfiguration::get();        
    wps = TTHbb::util::vectoriseString((*config)("MVAVariables.bTagWPs"));

    /// Path where to find the XML files
    m_xmlPath_1l = (*config)("AddBDT1LOS.XMLpath_1l"); 
    m_run1l = m_xmlPath_1l!="";
    if(!m_run1l) std::cout << "WARNING in TTHbb::AddBDT1LOS::initialise:\t"
			   << "XML path for 1l BDT not provided. Ignoring BDT 1l." << std::endl;
    else std::cout << "INFO in TTHbb::AddBDT1LOS::initialise:\t"
		   << "XML path for 1l BDT : " << m_xmlPath_1l << std::endl;


    m_xmlPath_2l = (*config)("AddBDT1LOS.XMLpath_2l");
    m_run2l = m_xmlPath_2l!="";
    if(!m_run2l) std::cout << "WARNING in TTHbb::AddBDT1LOS::initialise:\t"
			   << "XML path for 2l BDT not provided. Ignoring BDT 2l." << std::endl;
    else std::cout << "INFO in TTHbb::AddBDT1LOS::initialise:\t"
		   << "XML path for 2l BDT : " << m_xmlPath_2l << std::endl;

    // Cross-validation event sampling based on 3-fold cross validation
    EventSampling Fold1Sampling; // trained in eventNumber%3==0, apply on eventNumber%3==2
    Fold1Sampling.modulo = 3;
    Fold1Sampling.rest.push_back(2);
    Fold1Sampling.patternName = "cross_val_sample_0_BDTG";

    EventSampling Fold2Sampling; // trained in eventNumber%3==1, apply on eventNumber%3==1
    Fold2Sampling.modulo = 3;
    Fold2Sampling.rest.push_back(1);
    Fold2Sampling.patternName = "cross_val_sample_1_BDTG";

    EventSampling Fold3Sampling; // trained in eventNumber%3==2, apply on eventNumber%3==0
    Fold3Sampling.modulo = 3;
    Fold3Sampling.rest.push_back(0);
    Fold3Sampling.patternName = "cross_val_sample_2_BDTG";


    // BDT configuration for 1l
    if(m_run1l){

      // list of variables used by the BDT
      std::vector<std::string> vars_1l = TTHbb::util::vectoriseString((*config)("AddBDT1LOS.Vars_1l"));

      // 1l10ji3be
      TMVAConfiguration tmvaConfig_1l10ji;
      tmvaConfig_1l10ji.samplings.push_back(Fold1Sampling);
      tmvaConfig_1l10ji.samplings.push_back(Fold2Sampling);
      tmvaConfig_1l10ji.samplings.push_back(Fold3Sampling);
      tmvaConfig_1l10ji.name = "BDT_1l_score";
      tmvaConfig_1l10ji.setCalibFilePattern(m_xmlPath_1l+"/TMVA_4tops_1l_10ji3bi");
      tmvaConfig_1l10ji.setVariables(vars_1l);
      tmvaConfig_1l10ji.bookReader();
      m_classBDTCalculatorTool.addRegion(tmvaConfig_1l10ji, 10, -1, 0, 0, 1);

      // removing nJets for exclusive regions
      vars_1l.erase(std::find(vars_1l.begin(),vars_1l.end(),"nJets/I"));

      // 1l9je3bi
      if( !gSystem->AccessPathName( (m_xmlPath_1l+"/TMVA_4tops_1l_9je3bi_cross_val_sample_0_BDTG.weights.xml").c_str()) ){
	TMVAConfiguration tmvaConfig_1l9je;
	tmvaConfig_1l9je.samplings.push_back(Fold1Sampling);
	tmvaConfig_1l9je.samplings.push_back(Fold2Sampling);
	tmvaConfig_1l9je.samplings.push_back(Fold3Sampling);
	tmvaConfig_1l9je.name = "BDT_1l_score";
	tmvaConfig_1l9je.setCalibFilePattern(m_xmlPath_1l+"/TMVA_4tops_1l_9je3bi");
	tmvaConfig_1l9je.setVariables(vars_1l);
	tmvaConfig_1l9je.bookReader();
	m_classBDTCalculatorTool.addRegion(tmvaConfig_1l9je, 9, 9, 0, 0, 1);
      }
      else std::cout << "WARNING in TTHbb::AddBDT1LOS::initialise:\t"
		     << "1l9je3bi BDT XML files do not exist. Skipping it." << std::endl;
      
      // 1l8je3bi
      if( !gSystem->AccessPathName( (m_xmlPath_1l+"/TMVA_4tops_1l_8je3bi_cross_val_sample_0_BDTG.weights.xml").c_str()) ){
	TMVAConfiguration tmvaConfig_1l8je;
	tmvaConfig_1l8je.samplings.push_back(Fold1Sampling);
	tmvaConfig_1l8je.samplings.push_back(Fold2Sampling);
	tmvaConfig_1l8je.samplings.push_back(Fold3Sampling);
	tmvaConfig_1l8je.name = "BDT_1l_score";
	tmvaConfig_1l8je.setCalibFilePattern(m_xmlPath_1l+"/TMVA_4tops_1l_8je3bi");
	tmvaConfig_1l8je.setVariables(vars_1l);
	tmvaConfig_1l8je.bookReader();
	m_classBDTCalculatorTool.addRegion(tmvaConfig_1l8je, 8, 8, 0, 0, 1);
      }
      else std::cout << "WARNING in TTHbb::AddBDT1LOS::initialise:\t"
		     << "1l8je3bi BDT XML files do not exist. Skipping it." << std::endl;

    }

    // BDT configuration for 2l
    if(m_run2l){

      // list of variables used by the BDT
      std::vector<std::string> vars_2l = TTHbb::util::vectoriseString((*config)("AddBDT1LOS.Vars_2l"));

      // 2l8ji3bi
      TMVAConfiguration tmvaConfig_2l8ji;
      tmvaConfig_2l8ji.samplings.push_back(Fold1Sampling);
      tmvaConfig_2l8ji.samplings.push_back(Fold2Sampling);
      tmvaConfig_2l8ji.samplings.push_back(Fold3Sampling);
      tmvaConfig_2l8ji.name = "BDT_2l_score";
      tmvaConfig_2l8ji.setCalibFilePattern(m_xmlPath_2l+"/TMVA_4tops_2l_8ji3bi");
      tmvaConfig_2l8ji.setVariables(vars_2l);
      tmvaConfig_2l8ji.bookReader();
      m_classBDTCalculatorTool.addRegion(tmvaConfig_2l8ji, 8, -1, 0, 0, 2);

      // removing nJets for exclusive regions
      vars_2l.erase(std::find(vars_2l.begin(),vars_2l.end(),"nJets/I"));

      // 2l7je3bi
      if( !gSystem->AccessPathName( (m_xmlPath_2l+"/TMVA_4tops_2l_7je3bi_cross_val_sample_0_BDTG.weights.xml").c_str()) ){
	TMVAConfiguration tmvaConfig_2l7je;
	tmvaConfig_2l7je.samplings.push_back(Fold1Sampling);
	tmvaConfig_2l7je.samplings.push_back(Fold2Sampling);
	tmvaConfig_2l7je.samplings.push_back(Fold3Sampling);
	tmvaConfig_2l7je.name = "BDT_2l_score";
	tmvaConfig_2l7je.setCalibFilePattern(m_xmlPath_2l+"/TMVA_4tops_2l_7je3bi");
	tmvaConfig_2l7je.setVariables(vars_2l);
	tmvaConfig_2l7je.bookReader();
	m_classBDTCalculatorTool.addRegion(tmvaConfig_2l7je, 7, 7, 0, 0, 2);
      }
      else std::cout << "WARNING in TTHbb::AddBDT1LOS::initialise:\t"
		     << "2l7je3bi BDT XML files do not exist. Skipping it." << std::endl;

      // 2l6je3bi
      if( !gSystem->AccessPathName( (m_xmlPath_2l+"/TMVA_4tops_2l_6je3bi_cross_val_sample_0_BDTG.weights.xml").c_str()) ){
	TMVAConfiguration tmvaConfig_2l6je;
	tmvaConfig_2l6je.samplings.push_back(Fold1Sampling);
	tmvaConfig_2l6je.samplings.push_back(Fold2Sampling);
	tmvaConfig_2l6je.samplings.push_back(Fold3Sampling);
	tmvaConfig_2l6je.name = "BDT_2l_score";
	tmvaConfig_2l6je.setCalibFilePattern(m_xmlPath_2l+"/TMVA_4tops_2l_6je3bi");
	tmvaConfig_2l6je.setVariables(vars_2l);
	tmvaConfig_2l6je.bookReader();
	m_classBDTCalculatorTool.addRegion(tmvaConfig_2l6je, 6, 6, 0, 0, 2);
      }
      else std::cout << "WARNING in TTHbb::AddBDT1LOS::initialise:\t"
		     << "2l6je3bi BDT XML files do not exist. Skipping it." << std::endl;
    }

  }

  //____________________________________________________
  void AddBDT1LOS::apply(TTHbb::Event* event) {


    /// Variables needed for BDT computation/studies

    // Include MVA variables
    auto* tManager = TTHbb::ToolManager::get();
    OfflineMVAVariables* OffMVAVars = (OfflineMVAVariables*)tManager->getInstanceOfTool("MVAVariables");

    std::shared_ptr<MVAVariables> mva;

    for(auto wp : wps){

      mva = OffMVAVars->getMVAVariables(wp);
      mva->setBtagString("jet_isbtagged_"+wp);
      mva->initialiseEvent(event);

      GetOrderedDL1rVars(event);

      event->floatVariable("Centrality_bjets_"+wp) = mva->Centrality(collection::bjets);
      event->floatVariable("Aplanarity_bjets_"+wp) = mva->Aplanarity(collection::bjets);
      event->floatVariable("Sphericity_bjets_"+wp) = mva->Sphericity(collection::bjets);
      event->floatVariable("H0_bjets_"+wp) = mva->FirstFoxWolframMoment(collection::bjets);
      event->floatVariable("H2_bjets_"+wp) = mva->ThirdFoxWolframMoment(collection::bjets);

      event->floatVariable("Mbb_MinM_"+wp) = mva->MassofPair(pairing::bb, variable::MinM);
      event->floatVariable("Mbb_MaxM_"+wp) = mva->MassofPair(pairing::bb, variable::MaxM);
      event->floatVariable("Mbb_Avg_"+wp) = mva->AverageMassPair(pairing::bb);

      event->floatVariable("Mbb_HiggsMass_"+wp) = mva->MassofPair(pairing::bb, variable::HiggsMass);
      event->floatVariable("Mbb_MindR_"+wp) = mva->MassofPair(pairing::bb, variable::MindR);
      event->floatVariable("Mbb_MaxPt_"+wp) = mva->MassofPair(pairing::bb, variable::MaxPt);
      event->floatVariable("Mbb_MinPt_"+wp) = mva->MassofPair(pairing::bb, variable::MinPt);

      event->floatVariable("Mbbb_MinM_"+wp) = mva->MassofBJetTriplet(variable::MinM);
      event->floatVariable("Mbbb_MaxM_"+wp) = mva->MassofBJetTriplet(variable::MaxM);
      event->floatVariable("Mbbb_Avg_"+wp) = mva->AverageMassTriplet(collection::bjets);

      event->floatVariable("dRbb_MaxdR_"+wp) = mva->deltaRofPair(pairing::bb, variable::MaxdR);
      event->floatVariable("dRbb_MindR_"+wp) = mva->deltaRofPair(pairing::bb, variable::MindR);
      event->floatVariable("dRbb_Avg_"+wp) = mva->deltaRofPair(pairing::bb, variable::Avg);
      event->floatVariable("dRbb_MaxPt_"+wp) = mva->deltaRofPair(pairing::bb, variable::MaxPt);
      event->floatVariable("dRbb_MaxM_"+wp) = mva->deltaRofPair(pairing::bb, variable::MaxM);

      event->floatVariable("dRbl_MaxdR_"+wp) = mva->deltaRofPair(pairing::bl, variable::MaxdR);
      event->floatVariable("dRbl_MindR_"+wp) = mva->deltaRofPair(pairing::bl, variable::MindR);
      event->floatVariable("dRbl_Avg_"+wp) = mva->deltaRofPair(pairing::bl, variable::Avg);

      event->floatVariable("dEtabb_MaxdEta_"+wp) = mva->deltaEtaofPair(pairing::bb, variable::MaxdEta);
      event->floatVariable("dEtabb_MindEta_"+wp) = mva->deltaEtaofPair(pairing::bb, variable::MindEta);
      event->floatVariable("dEtabb_Avg_"+wp) = mva->deltaEtaofPair(pairing::bb, variable::Avg);

      event->floatVariable("dEtabl_MaxdEta_"+wp) = mva->deltaEtaofPair(pairing::bl, variable::MaxdEta);
      event->floatVariable("dEtabl_MindEta_"+wp) = mva->deltaEtaofPair(pairing::bl, variable::MindEta);
      event->floatVariable("dEtabl_Avg_"+wp) = mva->deltaEtaofPair(pairing::bl, variable::Avg);

      event->floatVariable("dPhibb_MaxdPhi_"+wp) = mva->deltaPhiofPair(pairing::bb, variable::MaxdPhi);
      event->floatVariable("dPhibb_MindPhi_"+wp) = mva->deltaPhiofPair(pairing::bb, variable::MindPhi);
      event->floatVariable("dPhibb_Avg_"+wp) = mva->deltaPhiofPair(pairing::bb, variable::Avg);

      event->floatVariable("dPhibl_MaxdPhi_"+wp) = mva->deltaPhiofPair(pairing::bl, variable::MaxdPhi);
      event->floatVariable("dPhibl_MindPhi_"+wp) = mva->deltaPhiofPair(pairing::bl, variable::MindPhi);
      event->floatVariable("dPhibl_Avg_"+wp) = mva->deltaPhiofPair(pairing::bl, variable::Avg);

      event->intVariable  ("nTopsbj_30_"+wp) = mva->nHiggsCandidatesMassWindow(pairing::bj, 30);
      event->floatVariable("pTbb_MindR_"+wp) = mva->PtofPair(pairing::bb, variable::MindR);
    }
      
    if(mva){
      event->floatVariable("Centrality_all") = mva->Centrality(collection::all);
      event->floatVariable("Centrality_jets") = mva->Centrality(collection::jets);
      event->floatVariable("Aplanarity_all") = mva->Aplanarity(collection::all);
      event->floatVariable("Aplanarity_jets") = mva->Aplanarity(collection::jets);
      event->floatVariable("Sphericity_all") = mva->Sphericity(collection::all);
      event->floatVariable("Sphericity_jets") = mva->Sphericity(collection::jets);
      event->floatVariable("H0_all") = mva->FirstFoxWolframMoment(collection::all);
      event->floatVariable("H0_jets") = mva->FirstFoxWolframMoment(collection::jets);
      event->floatVariable("H2_all") = mva->ThirdFoxWolframMoment(collection::all);
      event->floatVariable("H2_jets") = mva->ThirdFoxWolframMoment(collection::jets);

      event->floatVariable("Mjj_MinM") = mva->MassofPair(pairing::jj, variable::MinM);
      event->floatVariable("Mjj_MaxM") = mva->MassofPair(pairing::jj, variable::MaxM);
      event->floatVariable("Mjj_Avg") = mva->AverageMassPair(pairing::jj);

      event->floatVariable("Mjjj_MindR") = Get_Mjjj_MindR(event);
      event->floatVariable("Mjjj_AvgdRs3") = Get_Mjjj_AvgdRs3(event);
      event->floatVariable("Mjjj_MinM") = mva->MassofJetTriplet(variable::MinM);
      event->floatVariable("Mjjj_MaxM") = mva->MassofJetTriplet(variable::MaxM);
      event->floatVariable("Mjjj_Avg") = mva->AverageMassTriplet(collection::jets);
  
      event->floatVariable("dRjj_MaxdR") = mva->deltaRofPair(pairing::jj, variable::MaxdR);
      event->floatVariable("dRjj_MindR") = mva->deltaRofPair(pairing::jj, variable::MindR);
      event->floatVariable("dRjj_Avg") = mva->deltaRofPair(pairing::jj, variable::Avg);

      event->floatVariable("dRjl_MaxdR") = mva->deltaRofPair(pairing::jl, variable::MaxdR);
      event->floatVariable("dRjl_MindR") = mva->deltaRofPair(pairing::jl, variable::MindR);
      event->floatVariable("dRjl_Avg") = mva->deltaRofPair(pairing::jl, variable::Avg);

      event->floatVariable("dEtajj_MaxdEta") = mva->deltaEtaofPair(pairing::jj, variable::MaxdEta);
      event->floatVariable("dEtajj_MindEta") = mva->deltaEtaofPair(pairing::jj, variable::MindEta);
      event->floatVariable("dEtajj_Avg") = mva->deltaEtaofPair(pairing::jj, variable::Avg);

      event->floatVariable("dEtajl_MaxdEta") = mva->deltaEtaofPair(pairing::jl, variable::MaxdEta);
      event->floatVariable("dEtajl_MindEta") = mva->deltaEtaofPair(pairing::jl, variable::MindEta);
      event->floatVariable("dEtajl_Avg") = mva->deltaEtaofPair(pairing::jl, variable::Avg);

      event->floatVariable("dPhijj_MaxdPhi") = mva->deltaPhiofPair(pairing::jj, variable::MaxdPhi);
      event->floatVariable("dPhijj_MindPhi") = mva->deltaPhiofPair(pairing::jj, variable::MindPhi);
      event->floatVariable("dPhijj_Avg") = mva->deltaPhiofPair(pairing::jj, variable::Avg);

      event->floatVariable("dPhijl_MaxdPhi") = mva->deltaPhiofPair(pairing::jl, variable::MaxdPhi);
      event->floatVariable("dPhijl_MindPhi") = mva->deltaPhiofPair(pairing::jl, variable::MindPhi);
      event->floatVariable("dPhijl_Avg") = mva->deltaPhiofPair(pairing::jl, variable::Avg);

      event->intVariable  ("nWjj_15") = mva->nWCandidatesMassWindow(pairing::jj, 15);
    }

    // jet DL1r pseudo-continuous score discrete
    unsigned int njets = event->m_jets.size();
    event->floatVariable("jet_DL1r_ordered_discrete_2") = 
      njets>2 ? event->m_jets.at(2)->floatVariable("jet_discrete_DL1r_btag_ordered") : -100;
    event->floatVariable("jet_DL1r_ordered_discrete_3") = 
      njets>3 ? event->m_jets.at(3)->floatVariable("jet_discrete_DL1r_btag_ordered") : -100;
    event->floatVariable("jet_DL1r_ordered_discrete_4") = 
      njets>4 ? event->m_jets.at(4)->floatVariable("jet_discrete_DL1r_btag_ordered") : -100;

    // 1st rcjets kinematic
    unsigned int nRCjets = event->m_reclusteredJets.size();
    event->floatVariable("rcjet_d12_0") = 
      nRCjets>0 ? event->m_reclusteredJets.at(0)->floatVariable("rcjet_d12") : -100;
    event->floatVariable("rcjet_d23_0") = 
      nRCjets>0 ? event->m_reclusteredJets.at(0)->floatVariable("rcjet_d23") : -100;
    event->floatVariable("rcjet_pt_0") = 
      nRCjets>0 ? event->m_reclusteredJets.at(0)->floatVariable("rcjet_pt") : -100;

    float sum_rcjet_d12=0.0;
    for (int i=0;i<(int)nRCjets;i++)
      sum_rcjet_d12+=event->m_reclusteredJets.at(i)->floatVariable("rcjet_d12");
    event->floatVariable("Sum__rcjet_d12_") = sum_rcjet_d12;

    float sum_rcjet_d23=0.0;
    for (int i=0;i<(int)nRCjets;i++)
      sum_rcjet_d23+=event->m_reclusteredJets.at(i)->floatVariable("rcjet_d23");
    event->floatVariable("Sum__rcjet_d23_") = sum_rcjet_d23;

    float sum_jet_pcb=0.0;
    for (int i=0;i<(int)njets;i++)
      if (i<6)
	sum_jet_pcb+=event->m_jets.at(i)->intVariable("jet_pcb_DL1r_btag_ordered");
    event->floatVariable("Sum__jet_pcb_DL1r_btag_ordered_T__Iteration__6__") = sum_jet_pcb;

    event->floatVariable("jet_pt_0_") = event->m_jets.at(0)->Pt();

     // met_met being specificly defined as in the list of float variables 
    event->floatVariable("met_met") = event->met_met;

    // BDT score variable initialization
    event->floatVariable("BDT_1l_score") = -999;
    event->floatVariable("BDT_2l_score") = -999;    

    // BDT computation
    m_classBDTCalculatorTool.computeBDT(event, event->m_jets.size(), 0, event->m_leptons.size(), 
					event->m_info->eventNumber);    
  }

  //____________________________________________________    
  void AddBDT1LOS::finalise() {
  }

  //____________________________________________________
  void AddBDT1LOS::GetOrderedMV2Vars(std::shared_ptr<MVAVariables> mva, TTHbb::Event* event){

    std::vector< std::shared_ptr<TTHbb::Particle> > jets_dl1r = mva->getMV2OrderedJetContainer();
    for(uint i=0; i<jets_dl1r.size(); i++){
      std::shared_ptr<TTHbb::Jet> jet = event->m_jets.at(i);
      jet->floatVariable("jet_pt_btag_ordered") = jets_dl1r.at(i)->Pt();
      jet->floatVariable("jet_eta_btag_ordered") = jets_dl1r.at(i)->Eta();
      jet->floatVariable("jet_phi_btag_ordered") = jets_dl1r.at(i)->Phi();
      jet->floatVariable("jet_e_btag_ordered") = jets_dl1r.at(i)->E();
      jet->floatVariable("jet_DL1r_btag_ordered") = jets_dl1r.at(i)->floatVariable("jet_DL1r");
      jet->intVariable("jet_pcb_DL1r_btag_ordered") = jets_dl1r.at(i)->intVariable("jet_tagWeightBin_DL1r_Continuous");
      jet->floatVariable("jet_discrete_DL1r_btag_ordered") = GetFloatedPCB(jets_dl1r.at(i)->floatVariable("jet_DL1r"));
    }
  }

  //____________________________________________________
  void AddBDT1LOS::GetOrderedDL1rVars(TTHbb::Event* event){

    auto jetCopy = event->m_jets;

    std::sort(std::begin(jetCopy), std::end(jetCopy), 
	      [](std::shared_ptr<TTHbb::Jet> j1, std::shared_ptr<TTHbb::Jet> j2){
		return j1->intVariable("jet_tagWeightBin_DL1r_Continuous") > j2->intVariable("jet_tagWeightBin_DL1r_Continuous");
	      });

    for(uint i=0; i<jetCopy.size(); i++){
      std::shared_ptr<TTHbb::Jet> jet = event->m_jets.at(i);
      jet->floatVariable("jet_pt_btag_ordered") = jetCopy.at(i)->Pt();
      jet->floatVariable("jet_eta_btag_ordered") = jetCopy.at(i)->Eta();
      jet->floatVariable("jet_phi_btag_ordered") = jetCopy.at(i)->Phi();
      jet->floatVariable("jet_e_btag_ordered") = jetCopy.at(i)->E();
      jet->floatVariable("jet_DL1r_btag_ordered") = jetCopy.at(i)->floatVariable("jet_DL1r");
      jet->intVariable("jet_pcb_DL1r_btag_ordered") = jetCopy.at(i)->intVariable("jet_tagWeightBin_DL1r_Continuous");
      jet->floatVariable("jet_discrete_DL1r_btag_ordered") = GetFloatedPCB(jetCopy.at(i)->floatVariable("jet_DL1r"));
    }
  }

  //____________________________________________________
  float AddBDT1LOS::GetFloatedPCB(float dl1r){

    if ( dl1r < 0.11 ) 
      return ( ( -1 + 0.11 ) / 2 );
    else if ( dl1r >= 0.11 && dl1r < 0.64 ) //1758475
      return ( 0.11 + ( 0.64 - 0.11 ) / 2 );
    else if ( dl1r >= 0.64 && dl1r < 0.83 ) //0.645925
      return ( 0.64 + ( 0.83 - 0.64 ) / 2 );
    else if ( dl1r >= 0.83 && dl1r < 0.94 ) //0.8244273 
      return ( 0.83 + ( 0.94 - 0.83 ) / 2 );
    else if ( dl1r >= 0.94 ) //0.934906
      return ( 0.94 + ( 1 - 0.94 ) / 2 );

    return -99;
  }

  float AddBDT1LOS::Get_Mjjj_MindR(TTHbb::Event* event){
    
    float min_deltaR = FLT_MAX;
    float min_trijet_mass = 0;
    float current_trijet_mass = 0;
    float current_deltaR = 0;

    
    TLorentzVector tl_vec,tl_jet1,tl_jet2,tl_jet3;
    std::shared_ptr<Jet> jet1,jet2,jet3;
    
    unsigned int jets_n = event->m_jets.size();
    
    for(unsigned int i = 0; i < jets_n; i++){
      for(unsigned int k = 0; k < jets_n; k++){
	for(unsigned int h = 0; h < jets_n; h++){
	  if(i != k && i!=h && h != k){
	    jet1 = event->m_jets.at(i);
	    jet2 = event->m_jets.at(k);
	    jet3 = event->m_jets.at(h);
	    
	    tl_jet1 = GetTlorentzVector(jet1);
	    tl_jet2 = GetTlorentzVector(jet2);
	    tl_jet3 = GetTlorentzVector(jet3);
	    
	    tl_vec =tl_jet1 + tl_jet2 + tl_jet3;
	    current_trijet_mass = tl_vec.M();
	    current_deltaR = sqrt(pow(tl_jet1.DeltaR(tl_jet2),2) + pow(tl_jet1.DeltaR(tl_jet3),2) + pow(tl_jet2.DeltaR(tl_jet3),2));
	    
	    if(current_deltaR < min_deltaR){
	      min_deltaR=current_deltaR;
	      min_trijet_mass = current_trijet_mass;
	    }
	    
	  }
	}
      }
    }  
    return min_trijet_mass;
  }
  
  float AddBDT1LOS::Get_Mjjj_AvgdRs3(TTHbb::Event* event){
    
    float current_trijet_mass = 0;
    float current_deltaR = 0;
    float avg_mass = 0.0;
    unsigned int n_mass = 0;
    
    TLorentzVector tl_vec,tl_jet1,tl_jet2,tl_jet3;
    std::shared_ptr<Jet> jet1,jet2,jet3;
    
    unsigned int jets_n = event->m_jets.size();
    
    for(unsigned int i = 0; i < jets_n; i++){
      for(unsigned int k = 0; k < jets_n; k++){
	for(unsigned int h = 0; h < jets_n; h++){
	  if(i != k && i!=h && h != k){
	    jet1 = event->m_jets.at(i);
	    jet2 = event->m_jets.at(k);
	    jet3 = event->m_jets.at(h);
	    
	    tl_jet1 = GetTlorentzVector(jet1);
	    tl_jet2 = GetTlorentzVector(jet2);
	    tl_jet3 = GetTlorentzVector(jet3);
	    
	    tl_vec =tl_jet1 + tl_jet2 + tl_jet3;
	    current_trijet_mass = tl_vec.M();
	    current_deltaR = sqrt(pow(tl_jet1.DeltaR(tl_jet2),2) + pow(tl_jet1.DeltaR(tl_jet3),2) + pow(tl_jet2.DeltaR(tl_jet3),2));
	    
	    if(current_deltaR < 3){
	      avg_mass += current_trijet_mass;
	      n_mass++;
	    }
	    
	  }
	}
      }
    }  
    return avg_mass/static_cast<Double_t>(n_mass>0?n_mass:1);
  }

  TLorentzVector AddBDT1LOS::GetTlorentzVector(std::shared_ptr<Jet> jet){
    TLorentzVector vec;
    vec.SetPtEtaPhiE(jet->Pt(),jet->Eta(),jet->Phi(),jet->E());
    return vec;
  }


}
