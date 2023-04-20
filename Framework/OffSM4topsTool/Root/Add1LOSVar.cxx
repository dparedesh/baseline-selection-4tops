#include "OffSM4topsTool/Add1LOSVar.h"
#include "OffSM4topsTool/OffSM4topsConfiguration.h"
#include "TTHbbObjects/TTHbbUtils.h"

namespace TTHbb{

  // tests

  double topcand( std::shared_ptr<Jet>*threejets){

    double mass;
    for (int ii=0; ii<3; ii++){
       for (int jj=ii+1; jj<3; jj++){
             mass = (threejets[ii]->p4() + threejets[jj]->p4()).M();
             if (mass>78*1000 && mass<88*1000) return (threejets[0]->p4() +  threejets[1]->p4() + threejets[2]->p4()).M()*0.001;
       }
    }
    
    return -1;
  }




  // ----------------------------- public -----------------------------------------

  void Add1LOSVar::initialise(){
    // Check if input ntuple is data
    isData = false;
    auto * config = (OffSM4topsConfiguration*) TTHbb::OffSM4topsConfiguration::get();
    if(TTHbb::util::iequals((*config)("SampleType"),"data")) isData = true;

  }

  void Add1LOSVar::apply(TTHbb::Event* event){
    event->intVariable("SR") = signalRegion(event);
    event->intVariable("year") = Year(event);
    event->floatVariable("weight_lumi") = WeightLumi(event);

    AddRCJetMass(event);
    Get_dPhi_met_lep_vars(event);
    Get_met_dilep_vars(event);
    event->floatVariable("ptW") = Get_ptW(event);

    // If asked, convert MeV->GeV
    auto* config = TTHbb::GlobalConfiguration::get();
    if(TTHbb::util::iequals((*((OffSM4topsConfiguration*)config))("Add1LOSVar.MeVToGeV"),"true")) 
        Convert(event);
  }

  void Add1LOSVar::finalise(){
  }

  // ----------------------------- private -----------------------------------------
  
  int Add1LOSVar::signalRegion(TTHbb::Event* event){    

    // Todo: Implement Trigger Matching cuts

    // Extract the lepton channel flavor
    int lepChan = event->intVariable("lepFlavChannel");    

    // decorate event with useful infos, since we have infos at hands
    event->charVariable("pass_triangle") = false;
    event->charVariable("pass_slep")     = false;
    event->charVariable("pass_sf_cut")   = false;
    event->charVariable("pass_dilep")    = false;
    

    int Nj = event->intVariable("nJets");
    int NJ = event->intVariable("nRCJets");
    int Nb = event->intVariable("nBTags_DL1r_77");

    // Check 1L channel
    if( lepChan == 1 || lepChan == 2 ){ // 1L channel 

      event->charVariable("pass_slep") = (Nb>1 && Nj>3);

      // Pre-selection
      float met = event->met_met, mtw = event->floatVariable("mtw");
      if( met < 20000 || (met+mtw) < 60000 ) {
           return 0;
      }

      event->charVariable("pass_triangle") = (Nb>1 && Nj>3);
 
      // Signal region caterogization


      if( Nb==3 and Nj==9 and NJ==0 ) return 1;
      else if( Nb==3 and Nj==9 and NJ==1 ) return 2;
      else if( Nb==3 and Nj==9 and NJ>1 ) return 3;
      else if( Nb>3 and Nj==9 and NJ==0 ) return 4;
      else if( Nb>3 and Nj==9 and NJ==1 ) return 5;
      else if( Nb>3 and Nj==9 and NJ>1 ) return 6;
      else if( Nb==3 and Nj>9 and NJ==0 ) return 7;
      else if( Nb==3 and Nj>9 and NJ==1 ) return 8;
      else if( Nb==3 and Nj>9 and NJ>1 ) return 9;
      else if( Nb>3 and Nj>9 and NJ==0 ) return 10;
      else if( Nb>3 and Nj>9 and NJ==1 ) return 11;
      else if( Nb>3 and Nj>9 and NJ>1 ) return 12;
      else return 0;
    }    

    // Check 2LOS channel
    else if( lepChan == 3 || lepChan == 4 || lepChan == 5 ){ // 2LOS channel 

      event->charVariable("pass_dilep")    = (Nb>1 && Nj>3);

      // Pre-selection
      if( lepChan == 3 || lepChan == 4 ){ 
	float mll = event->floatVariable("mll");
	if( mll<15000 || (mll>83000 && mll<99000) ) return 0;
      }

      event->charVariable("pass_sf_cut")   = (Nb>1 && Nj>3);

      // Signal region caterogization
      if( Nb==3 and Nj==7 and NJ==0 ) return 13;
      else if( Nb==3 and Nj==7 and NJ>0 ) return 14;
      else if( Nb>3 and Nj==7 and NJ==0 ) return 15;
      else if( Nb>3 and Nj==7 and NJ>0 ) return 16;
      else if( Nb==3 and Nj>7 and NJ==0 ) return 17;
      else if( Nb==3 and Nj>7 and NJ>0 ) return 18;
      else if( Nb>3 and Nj>7 and NJ==0 ) return 19;
      else if( Nb>3 and Nj>7 and NJ>0 ) return 20;
      else return 0;
    }

    return 0;
  }

  int Add1LOSVar::Year(TTHbb::Event* event){    

    int runNumber = event->m_info->runNumber;
    if (isData){
      if      ( runNumber >= 348885 ) return 2018;
      else if ( runNumber >= 325713 && runNumber <= 340453 ) return 2017;
      else if ( runNumber >= 297730 && runNumber <= 311481 ) return 2016;
      else if ( runNumber >= 276262 && runNumber <= 284484 ) return 2015;
    }
    else {
      if      ( runNumber == 284500) return 2016; // mc16a, set manually only to year 2016; distinguishing between '15 and '16 through randomrunNumber has become obsolete.
      else if ( runNumber == 300000) return 2017;
      else if ( runNumber == 310000) return 2018;
    }

    return 1985;
  }

  float Add1LOSVar::WeightLumi(TTHbb::Event* event){

    int runNumber=event->m_info->runNumber;
    if(!isData){
      if      ( runNumber == 284500) return (3219.56+32988.1)/138965.16;
      else if ( runNumber == 300000) return 44307.4/138965.16;
      else if ( runNumber == 310000) return 58450.1/138965.16;
    }

    return 1.;
  }

  void Add1LOSVar::Convert(TTHbb::Event* event){
    float m_HT_jets = event->floatVariable("HT_jets");
    event->floatVariable("HT_jets") = m_HT_jets*0.001;
    std::shared_ptr<Jet> jet,jet2,jet3;

    unsigned int jets_n = event->m_jets.size();
    if (jets_n==0) std::cout << "WARNING: No jets in event! " << std::endl;

    auto* config = (OffSM4topsConfiguration*) TTHbb::OffSM4topsConfiguration::get();
    std::shared_ptr<Jet> threejets[3];              
    int ncomb=0;
    double candmass=-999,avecandmass=0;
    for(unsigned int i = 0; i < jets_n; i++){
      jet = event->m_jets.at(i);
      // jet->SetPtEtaPhiE(jet->Pt()*0.001,jet->Eta(),jet->Phi(),jet->E()*0.001);
      jet->floatVariable("jet_pt_GeV")=jet->Pt()*0.001;
      jet->floatVariable("jet_e_GeV")=jet->E()*0.001;

      // If running TtTRF, be kind and translate (FIXME)
      if(TTHbb::util::iequals((*config)("TtTRF.TurnOn"),"true")) {
        jet->floatVariable("jet_btag_weight")=jet->floatVariable("jet_DL1r");
      }

      threejets[0]=jet;
      for(unsigned int j = i+1; j < jets_n; j++){
      threejets[1]= event->m_jets.at(j);
         for(unsigned int k = j+1; k < jets_n; k++){
           threejets[2]= event->m_jets.at(k);
           candmass = topcand(threejets);
           if ( candmass>150 and candmass<180 ) {
               ncomb++;
               avecandmass+=candmass;
           }
         }
      }

    }
    event->intVariable("nComb")=ncomb;
    event->floatVariable("aveCandTopMass")=avecandmass/ncomb;
  }


  TLorentzVector Add1LOSVar::GetTlorentzVector(std::shared_ptr<Jet> jet){
    TLorentzVector vec;
    vec.SetPtEtaPhiE(jet->Pt(),jet->Eta(),jet->Phi(),jet->E());
    return vec;
  }
  
  TLorentzVector Add1LOSVar::GetTlorentzVector(std::shared_ptr<Lepton> lep){
    TLorentzVector vec;
    vec.SetPtEtaPhiE(lep->Pt(),lep->Eta(),lep->Phi(),lep->E());
    return vec;
  }

  TLorentzVector Add1LOSVar::GetTlorentzVector(std::shared_ptr<ReclusteredJet> rcjet){
    TLorentzVector vec;
    vec.SetPtEtaPhiE(rcjet->Pt(),rcjet->Eta(),rcjet->Phi(),rcjet->E());
    return vec;
  }

  void Add1LOSVar::AddRCJetMass(TTHbb::Event* event){
    auto rcjets = event->m_reclusteredJets;
    for (std::shared_ptr<ReclusteredJet> rcjet: rcjets){
      TLorentzVector vrcjet = GetTlorentzVector(rcjet);
      rcjet->floatVariable("rcjet_m") = vrcjet.M();
    }
  }

  void Add1LOSVar::Get_met_dilep_vars(TTHbb::Event* event){

    if(event->m_leptons.size() > 1){
     TLorentzVector lep1 = event->m_leptons.at(0)->p4();
     TLorentzVector lep2 = event->m_leptons.at(1)->p4();
     TLorentzVector met; met.SetPtEtaPhiM(event->met_met,0,event->met_phi,1);
     TLorentzVector comb = lep1 + lep2 + met;
     event->floatVariable("M_dilep_met") = comb.M();
     event->floatVariable("Pt_dilep_met") = comb.Pt();
   }
   else {
     event->floatVariable("M_dilep_met") = -999;
     event->floatVariable("Pt_dilep_met") = -999;
   }

  }

  void Add1LOSVar::Get_dPhi_met_lep_vars(TTHbb::Event* event){

    // first lepton
    double dphi0 = TVector2::Phi_mpi_pi(event->m_leptons.at(0)->Phi()-event->met_phi);
    event->floatVariable("dPhi_met_lep_Max") = dphi0;
    event->floatVariable("dPhi_met_lep_Min") = dphi0;
    event->floatVariable("dPhi_met_lep_Avg") = dphi0;

    // second lepton
    if(event->m_leptons.size()>1){
      double dphi1 = TVector2::Phi_mpi_pi(event->m_leptons.at(1)->Phi()-event->met_phi);
      if(dphi1>dphi0) event->floatVariable("dPhi_met_lep_Max") = dphi1;
      else event->floatVariable("dPhi_met_lep_Min") = dphi1;
      event->floatVariable("dPhi_met_lep_Avg") = (dphi0+ dphi1)/2;
    }

  }

  float Add1LOSVar::Get_ptW(TTHbb::Event* event){

    TLorentzVector tl_lep = event->m_leptons.at(0)->p4();
    TLorentzVector tl_met; tl_met.SetPtEtaPhiM(event->met_met,0,event->met_phi,1);

    return (tl_lep + tl_met).Pt();
  }
  
}

