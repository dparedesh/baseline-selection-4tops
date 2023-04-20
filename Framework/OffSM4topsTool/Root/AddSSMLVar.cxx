#include "OffSM4topsTool/AddSSMLVar.h"
#include <bitset>  // std::bitset. Use for readability
#include <fstream>
#include "PathResolver/PathResolver.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"
#include "OffSM4topsTool/OffSM4topsConfiguration.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH2D.h>
#include <TString.h>
#include <TH3F.h>
#include <TH3D.h>
#include <algorithm>
#include <functional>

// include for the Sphericity
#include <vector>
#include <complex>
#include <cmath>
#include <Eigen/Eigenvalues>
#include <stdlib.h>

// include for truth mass
#include <random>


namespace TTHbb {

// ----------------------------- public -----------------------------------------

void AddSSMLVar::initialise() {
  // Check if input ntuple is data
  isData = false;
  auto* config = TTHbb::GlobalConfiguration::get();
  if (TTHbb::util::iequals((*config)("SampleType"), "data")) isData = true;


  // Background categorisation
  m_truthHelper = new SM4t::TruthHelpers();



  //for charge flip
  doChargeFlip = false;
  auto* config4Tops = (OffSM4topsConfiguration*) TTHbb::OffSM4topsConfiguration::get();
  if (TTHbb::util::iequals((*config4Tops)("AddSSMLVar.doChargeFlip"),"true")) doChargeFlip=true;

  if (doChargeFlip){
    TString path_files = "/eos/user/s/ssmlbsm4tops/home-ssmlbsm4tops/AB_212120/QmisIDRates/";

    TString inputfile = path_files+"/QmisIDrates.Likelihood.data2015_2018.AB212120.Data_plus_nJets_ge0.root";
    TString ttWCRratefile = path_files+ "/QmisIDrates_CRttW.Likelihood.data2015_2018.AB212120.Data_plus_nJets_ge0.root";
    TString COCR3Dratefile_2eta = path_files+"/Likelihood_3D_2etabins.root";

    TString syst_default = path_files +"/QmisIDsys.AB212120.plus.root";
    TString syst_CRttW = path_files +"/QmisIDsys_CRttW.AB212120.plus.root";
    TString COCR3Dratefile_2eta_syst = path_files+"/Likelihood_3D_2etabins_syst.root";

    std::cout << " - Will compute charge flip weights" << std::endl;



    TFile* f0 = new TFile(inputfile,"read");
    m_qmisid_def = (TH2D*)f0->Get("Data_plus_nJets_ge0_all_num")->Clone();

    TFile* f2 = new TFile(ttWCRratefile,"read");
    m_qmisid_ttWCR = (TH2D*)f2->Get("Data_plus_nJets_ge0_all_num")->Clone();

    TFile* f1_2eta = new TFile(COCR3Dratefile_2eta,"read");
    m_qmisid_COCR_2eta = (TH3D*)f1_2eta->Get("Data_Default_nJets_ge0_all_num")->Clone();


    //Getting systematic histos:
    TFile *f0_syst =  new TFile(syst_default,"read");
    m_qmisid_def_syst = (TH2D*)f0_syst->Get("TotUncer")->Clone();

    TFile* f2_syst = new TFile(syst_CRttW,"read");
    m_qmisid_ttWCR_syst = (TH2D*)f2_syst->Get("TotUncer")->Clone();

    TFile* f1_2eta_syst = new TFile(COCR3Dratefile_2eta_syst,"read");
    m_qmisid_COCR_2eta_syst = (TH3D*)f1_2eta_syst->Get("TotUncer")->Clone();

  }


  // Apply TMVA
  m_MVAApplicator = new SM4t::SSMLMVAApplication();
  m_MVAApplicator->initialise();



}

void AddSSMLVar::apply(TTHbb::Event* event) {
  // Add truth mass and signal/bkg labels (metsai)
  ClassifySigBkg(event);

  // Add the variable: deltaR between lepton and clostest jet
  AddLeptonVariables(event);

  // Add jet angular variables
  AddJetVariables(event);

  // Add weight jet multiplicity
  WeightJetMultiplicity(event);

  // Add shape variables (metsai)
  Sphericity(event);

  // Apply BDT to event
  m_MVAApplicator->ApplyBDT(event);


}

void AddSSMLVar::finalise() {

  m_MVAApplicator->finalise();

}


////////////////////////////
//   Add sig/bkg labels   //
////////////////////////////

// metsai - To make the random mass consistent in each version of the offline production
void AddSSMLVar::ClassifySigBkg(TTHbb::Event* event){
    int isSignalBkg(0);
    auto  mcChannelNumber = (int)(event->m_info->mcChannelNumber);
    if (mcChannelNumber==312440 || mcChannelNumber==312441 || mcChannelNumber==312442 || mcChannelNumber==312443 || mcChannelNumber==312444 || mcChannelNumber==312445 || mcChannelNumber==312446){
      isSignalBkg = 1;
    }
    else{
        isSignalBkg = 0;
    }
    event->floatVariable("isSignalBkg") = isSignalBkg;
}
//metsai

// ----------------------------- private -----------------------------------------
void AddSSMLVar::AddLeptonVariables(TTHbb::Event* event) {

  // Declare some jet vars
  std::shared_ptr<Jet> jet;
  unsigned int jets_n = event->m_jets.size();
  TLorentzVector jets_vectorsum(0, 0, 0, 0);
  TLorentzVector bjets_vectorsum(0,0,0,0);
  std::vector<TLorentzVector> all_bjets = {};
  float jets_sum_pt(0);
  float bjets_sum_pt(0);

  // Declare some lepton vars
  std::shared_ptr<Lepton> lepton;
  std::shared_ptr<Lepton> lepton1;
  leptonType m_leptonType;
  std::vector<leptonType> event_leptonType;
  std::vector<int> leptons_charge;
  std::vector<TLorentzVector> all_leptons = {};
  std::vector<TLorentzVector> tight_electrons = {}; //save tight el for charge flip weight
  TLorentzVector leptons_vectorsum(0, 0, 0, 0);
  float leptons_sum_pt(0);


  int total_charge(0.);
  int N_OSSF(0);


  // Declare the angular variable between leptons
  float deltaR_ll_max(0);
  float deltaR_ll_min(100);
  float deltaR_ll_sum(0);
  float m_min_deltaR_lj(0);
  float m_min_deltaR_lb(0);

  float deltaR_lj_min(100);
  float deltaR_lb_min(100);
  float deltaR_lb_max(0);


  // Declare angular variables using exclusively tight leptons:
  float tight_deltaR_ll_max(0);
  float tight_deltaR_ll_min(100);
  float tight_deltaR_ll_sum(0);
  float tight_m_min_deltaR_lj(0);
  float tight_m_min_deltaR_lb(0);

  float tight_deltaR_lj_min(100);
  float tight_deltaR_lb_min(100);
  float tight_deltaR_lb_max(0);


                                                          //##  Daniela
  event->floatVariable("lepton_2_deltaR_lj_min") = -100;  //Where are the other lepton_*_ variables for this block?
  event->floatVariable("lepton_2_deltaR_lb_min") = -100;  //Event will have at least 2 leptons, and vars for lepton 0 and 1
  event->floatVariable("lepton_2_deltaR_lb_max") = -100;  // will be filled for sure. However, if nLep==2, those vars will
  event->floatVariable("lepton_2_m_deltaR_lj_min") = 0;   //be empty. We need to set a default value for them.
  event->floatVariable("lepton_2_m_deltaR_lb_min") = 0;   // Why not saying just SetEventLevelLeptonVar(event, "float", "deltaR_lj_min", "lepton_"); as done for the variables below?


  // Reset some event variables
  event->intVariable("SSee_passEtaCut") = 0;
  event->intVariable("SSem_passEtaCut") = 0;
  event->intVariable("SSee_passECIDS") = 0;
  event->intVariable("SSem_passECIDS") = 0;


  if (event->intVariable("SSee") == 1) {
    event->intVariable("SSee_passEtaCut") = 1;
    event->intVariable("SSee_passECIDS") = 1;
  }
  else if (event->intVariable("SSem") == 1) {
    event->intVariable("SSem_passEtaCut") = 1;
    event->intVariable("SSem_passECIDS") = 1;
  }


  event->floatVariable("el_MeePVCO") = -999;
  event->floatVariable("Tel_MeePVCO") = -999;

 // if (doChargeFlip){

    event->intVariable("OSee_passEtaCut") = 0;
    event->intVariable("OSem_passEtaCut") = 0;
    event->intVariable("OSee_passECIDS") = 0;
    event->intVariable("OSem_passECIDS") = 0;

    if (event->intVariable("OSem") == 1) {
      event->intVariable("OSem_passEtaCut") = 1;
      event->intVariable("OSem_passECIDS") = 1;
    }
    else if (event->intVariable("OSee") == 1) {
      event->intVariable("OSee_passEtaCut") = 1;
      event->intVariable("OSee_passECIDS") = 1;
    }
//  }



  // Set trigger match
  event->charVariable("DLTnoisoSLT") =  SetDLTrigger(event); //need to do it before the loop!


  event->charVariable("TrigMatch") = false; //per lepton



  // Set CRFlag
  event->intVariable("ttW_CRFlag") = 0;
  if (event->intVariable("SSee") == 1 || event->intVariable("SSem") == 1 ||
      event->intVariable("SSmm") == 1)
    event->intVariable("ttW_CRFlag") = 1;


  // Adding truth related variables
  std::vector<SM4t::TruthHelpers::SM4tTruthCategoryFlag> Lepton_flag;

  SetEventLevelLeptonVar(event, "int", "truthClassificationSM4t_bkgFlag", "lepton_");

  /*
  SetEventLevelLeptonVar(event, "char", "truthClassificationIFF_isFromPromptSource", "lepton_");
  SetEventLevelLeptonVar(event, "char", "truthClassificationIFF_isFromSMPromptSource", "lepton_");
  SetEventLevelLeptonVar(event, "char", "truthClassificationIFF_isFromLightSource", "lepton_");
  SetEventLevelLeptonVar(event, "char", "truthClassificationIFF_hasBHadronOrigin", "lepton_");
  SetEventLevelLeptonVar(event, "char", "truthClassificationIFF_hasCHadronOrigin", "lepton_");
  SetEventLevelLeptonVar(event, "char", "truthClassificationIFF_isPromptElectron", "lepton_");
  SetEventLevelLeptonVar(event, "char", "truthClassificationIFF_isChargeFlip", "lepton_");
  */

  // Adding other lepton variables
  SetEventLevelLeptonVar(event, "char", "isTight", "lepton_");
  SetEventLevelLeptonVar(event, "int", "ID", "lepton_");
  SetEventLevelLeptonVar(event, "float", "mll_atPV", "lepton_");
  SetEventLevelLeptonVar(event, "float", "mll_atConV", "lepton_");
  SetEventLevelLeptonVar(event, "float", "RadiusCO", "lepton_");


  // Adding exclusive vars for tight leptons_
  SetEventLevelLeptonVar(event, "int", "ID", "Tlepton_");
  SetEventLevelLeptonVar(event, "float", "mll_atPV", "Tlepton_");
  SetEventLevelLeptonVar(event, "float", "mll_atConV", "Tlepton_");
  SetEventLevelLeptonVar(event, "int", "truthClassificationSM4t_bkgFlag", "Tlepton_");
  SetEventLevelLeptonVar(event, "float" ,"pT", "Tlepton_");
  SetEventLevelLeptonVar(event, "float", "d0sig", "Tlepton_");
  SetEventLevelLeptonVar(event, "float", "delta_z0_sintheta", "Tlepton_");
  SetEventLevelLeptonVar(event, "int", "charge", "Tlepton_");



  // count number of tight leptons
  int nTight(0);
  int nTightChannel(0);

  // Getting nLeptons
  unsigned int leptons_n = event->m_leptons.size();

  //Start loop on leptons
  for (unsigned int i = 0; i < leptons_n; i++) {

    //setting some vars
    float deltaR_lj_min_i(100.);
    float deltaR_lb_min_i(100.);
    float deltaR_lb_max_i(0.);
    float m_lj_min_i(0);
    float m_lb_min_i(0);

    //getting lepton
    lepton = event->m_leptons.at(i);
    m_leptonType = lepton->type();

    event_leptonType.push_back(m_leptonType);
    leptons_charge.push_back(lepton->charge());


    //////////////////////////////
    // lepton ID and tight vars //
    //////////////////////////////

    bool lepton_isTight = (m_leptonType == leptonType::electron)
                              ? lepton->charVariable("el_isTight")
                              : lepton->charVariable("mu_isTight");

    if (i < 4) // consider only first 3 leptons: they are already ordered in pT
      event->charVariable("lepton_" + std::to_string(i) + "_isTight") = lepton_isTight;

    event->intVariable("lepton_" + std::to_string(i) + "_ID") =
        (m_leptonType == leptonType::electron) ? 11 : 13;


    ////////////////////////////////////////////////////////////
    // Event vars: need to build electron and muon separately //
    // /////////////////////////////////////////////////////////

    // Electron
    if (m_leptonType == leptonType::electron) {

      //filling passECIDS and passEta cuts
      if (event->intVariable("SSee") == 1 || event->intVariable("SSem") == 1 || event->intVariable("OSee") == 1 || event->intVariable("OSem") == 1) {

        if (!lepton->charVariable("el_ECIDS") && lepton->charVariable("el_isTight")) {

          if (event->intVariable("SSee") == 1) event->intVariable("SSee_passECIDS") = 0;
          if (event->intVariable("SSem") == 1) event->intVariable("SSem_passECIDS") = 0;
          if (event->intVariable("OSee") == 1) event->intVariable("OSee_passECIDS") = 0;
          if (event->intVariable("OSem") == 1) event->intVariable("OSem_passECIDS") = 0;

        }
        if (fabs(lepton->Eta()) > 1.37 && lepton->charVariable("el_isTight")) {

          if (event->intVariable("SSee") == 1) event->intVariable("SSee_passEtaCut") = 0;
          if (event->intVariable("SSem") == 1) event->intVariable("SSem_passEtaCut") = 0;
          if (event->intVariable("OSee") == 1) event->intVariable("OSee_passEtaCut") = 0;
          if (event->intVariable("OSem") == 1) event->intVariable("OSem_passEtaCut") = 0;

        }
      }

      TLorentzVector lepClosetTrack(0,0,0,0);
      TLorentzVector lepBestTrack(0,0,0,0);

      if (lepton->floatVariable("el_closestSiTrackPt")>0){
        lepClosetTrack.SetPtEtaPhiM(lepton->floatVariable("el_closestSiTrackPt"),lepton->floatVariable("el_closestSiTrackEta"),lepton->floatVariable("el_closestSiTrackPhi"),0.511);
        lepBestTrack.SetPtEtaPhiM(lepton->floatVariable("el_bestmatchSiTrackPt"),lepton->floatVariable("el_bestmatchSiTrackEta"),lepton->floatVariable("el_bestmatchSiTrackPhi"),0.511);

        // Maybe fill those variables just for tight leptons?  Daniela
        event->floatVariable("lepton_" + std::to_string(i) + "_mll_atPV") = (lepClosetTrack+lepBestTrack).M()*1e-3;
        lepClosetTrack.SetPtEtaPhiM(lepton->floatVariable("el_closestSiTrackPt"),lepton->floatVariable("el_closestSiTrackEta"),0,0.511);
        lepBestTrack.SetPtEtaPhiM(lepton->floatVariable("el_bestmatchSiTrackPt"),lepton->floatVariable("el_bestmatchSiTrackEta"),0,0.511);
        event->floatVariable("lepton_" + std::to_string(i) + "_mll_atConV") =(lepClosetTrack+lepBestTrack).M()*1e-3;
        event->floatVariable("lepton_" + std::to_string(i) + "_RadiusCO") = lepton->floatVariable("el_RadiusCO"); //Maybe to be out ?  ## Daniela
      }
    }// end if lep=electron


    ////////////////////////
    // Set trigger match  //
    ////////////////////////

    if (!event->charVariable("TrigMatch")){
      event->charVariable("TrigMatch") = SetLTriggerMatching(lepton,event->intVariable("runYear"));
    }


    //////////////////////
    // Define ttW flag  //
    //////////////////////

    if (event->intVariable("SSee") == 1 || event->intVariable("SSem") == 1 ||
        event->intVariable("SSmm") == 1) {
      if (m_leptonType == leptonType::electron &&
          lepton->floatVariable("el_PromptLeptonVeto") > -0.9 && lepton->charVariable("el_isTight"))
        event->intVariable("ttW_CRFlag") = 0;
      if (m_leptonType == leptonType::muon && lepton->floatVariable("mu_PromptLeptonVeto") > -0.9 &&
          lepton->charVariable("mu_isTight"))
        event->intVariable("ttW_CRFlag") = 0;
    }


    //////////////////////
    // DeltaR variables //
    //////////////////////

    TLorentzVector leptonFullM;
    leptonFullM.SetPtEtaPhiE(lepton->Pt(), lepton->Eta(), lepton->Phi(), lepton->E());

    all_leptons.push_back(leptonFullM);
    leptons_vectorsum += leptonFullM;
    leptons_sum_pt += lepton->Pt();

    //Get deltaR wrt jets
    for (unsigned int j = 0; j < jets_n; j++) {

      jet = event->m_jets.at(j);
      TLorentzVector jetFullM;
      jetFullM.SetPtEtaPhiE(jet->Pt(), jet->Eta(), jet->Phi(), jet->E());

      if (i == 0) jets_vectorsum += jetFullM; //Only for the first lepton, and do not repeat for others!
      if(i==0) jets_sum_pt+=jet->Pt();

      float temp_deltaR = leptonFullM.DeltaR(jetFullM); //get deltaR between lepton and jet

      // for each lepton, get the closest jet
      if (deltaR_lj_min_i > temp_deltaR) {
        deltaR_lj_min_i = temp_deltaR;
        m_lj_min_i = (leptonFullM + jetFullM).M();
      }
      // for the whole event, get the minimum deltaR between a lepton and a jet
      if (deltaR_lj_min > temp_deltaR) {
        deltaR_lj_min = temp_deltaR;
        m_min_deltaR_lj = (leptonFullM + jetFullM).M();
      }
      // for the whole event, but using only tight leptons
      if (lepton_isTight && tight_deltaR_lj_min > temp_deltaR) {
        tight_deltaR_lj_min = temp_deltaR;
        tight_m_min_deltaR_lj = (leptonFullM + jetFullM).M();
      }

      // std::cout<<jet->floatVariable("jet_DL1r")<<std::endl;
      //std::cout << " jet b-tag : " << jet->charVariable("jet_isbtagged_DL1r_77") << std::endl;

      if (jet->charVariable("jet_isbtagged_DL1r_77")) { // same as above but for b-jets

        if (deltaR_lb_min > temp_deltaR) {
          deltaR_lb_min = temp_deltaR;
          m_min_deltaR_lb = (leptonFullM + jetFullM).M();
        }

        if (lepton_isTight && tight_deltaR_lb_min > temp_deltaR) {
          tight_deltaR_lb_min = temp_deltaR;
          tight_m_min_deltaR_lb = (leptonFullM + jetFullM).M();
        }

        if (deltaR_lb_max < temp_deltaR) deltaR_lb_max = temp_deltaR;
        if (lepton_isTight && tight_deltaR_lb_max < temp_deltaR) tight_deltaR_lb_max = temp_deltaR;


        //fill for each lepton
        if (deltaR_lb_min_i > temp_deltaR) {
          deltaR_lb_min_i = temp_deltaR;
          m_lb_min_i = (leptonFullM + jetFullM).M();
        }

        if (deltaR_lb_max_i < temp_deltaR) deltaR_lb_max_i = temp_deltaR;

        if (i == 0){
           all_bjets.push_back(jetFullM);
           bjets_sum_pt += jet->Pt();
           bjets_vectorsum += jetFullM;
        }
      }
    }// end loop over jets

    // Lennart: do you call this lepton_ because you're also including loose leptons, unlike the
    // lep_ variables? I guess then it would be nice to have a `lepton_isTight`.
    // saving lepton level deltaR vars
    event->floatVariable("lepton_" + std::to_string(i) + "_deltaR_lj_min") = deltaR_lj_min_i;
    event->floatVariable("lepton_" + std::to_string(i) + "_deltaR_lb_min") = deltaR_lb_min_i;
    event->floatVariable("lepton_" + std::to_string(i) + "_deltaR_lb_max") = deltaR_lb_max_i;
    event->floatVariable("lepton_" + std::to_string(i) + "_m_deltaR_lj_min") = m_lj_min_i;
    event->floatVariable("lepton_" + std::to_string(i) + "_m_deltaR_lb_min") = m_lb_min_i;

    // Get deltaR wrt other leptons
    float charge_1 = lepton->charge();
    for (unsigned int k = i + 1; k < leptons_n; k++) {
      lepton1 = event->m_leptons.at(k);

      leptonType m_leptonType1 = lepton1->type();
      float charge_2 = lepton1->charge();

      bool lepton1_isTight = (m_leptonType1 == leptonType::electron)
                              ? lepton1->charVariable("el_isTight")
                              : lepton1->charVariable("mu_isTight");


      if (m_leptonType == m_leptonType1 && charge_1 * charge_2 < 0 && leptons_n >= 3)  N_OSSF++;


      TLorentzVector leptonFullM1;
      leptonFullM1.SetPtEtaPhiE(
          lepton1->Pt(), lepton1->Eta(), lepton1->Phi(),
          lepton1->E());  // Lennart: Why are you not using the leptons p4 directly?

      float deltaR_temp = leptonFullM.DeltaR(leptonFullM1);
      deltaR_ll_sum += deltaR_temp;

      if (deltaR_ll_max < deltaR_temp) deltaR_ll_max = deltaR_temp;
      if (deltaR_ll_min > deltaR_temp) deltaR_ll_min = deltaR_temp;

      // filling for only tight leptons
      if (lepton_isTight && lepton1_isTight){
        if (tight_deltaR_ll_max < deltaR_temp) tight_deltaR_ll_max = deltaR_temp;
        if (tight_deltaR_ll_min > deltaR_temp) tight_deltaR_ll_min = deltaR_temp;
        tight_deltaR_ll_sum += deltaR_temp;
      }

    }



    ////////////////////////////////////
    //Filling lepton truth variables  //
    ////////////////////////////////////

    if (!isData && i < 3) { //only the first 3 leptons must be counted for classification

      /*auto res = m_truthHelper->leptonCategorisationIFF(
          lepton);  // In order to store a Ulong_t to store the bitword, we would need
                    // to change some things in ttHbb. So split the bitword for now.

      event->charVariable("lepton_" + std::to_string(i) +
                          "_truthClassificationIFF_isFromPromptSource") = res.test(0);
      event->charVariable("lepton_" + std::to_string(i) +
                          "_truthClassificationIFF_isFromSMPromptSource") = res.test(1);
      event->charVariable("lepton_" + std::to_string(i) +
                          "_truthClassificationIFF_isFromLightSource") = res.test(2);
      event->charVariable("lepton_" + std::to_string(i) +
                          "_truthClassificationIFF_hasBHadronOrigin") = res.test(3);
      event->charVariable("lepton_" + std::to_string(i) +
                          "_truthClassificationIFF_hasCHadronOrigin") = res.test(4);
      event->charVariable("lepton_" + std::to_string(i) +
                          "_truthClassificationIFF_isPromptElectron") = res.test(5);
      event->charVariable("lepton_" + std::to_string(i) + "_truthClassificationIFF_isChargeFlip") =
          res.test(6);
      */

      SM4t::TruthHelpers::SM4tTruthCategoryFlag bkgFlag = m_truthHelper->leptonBkgFlagSM4t(lepton);

      event->intVariable("lepton_" + std::to_string(i) + "_truthClassificationSM4t_bkgFlag") =
          bkgFlag;
      if (lepton_isTight) {
        Lepton_flag.push_back(bkgFlag);
        total_charge += lepton->charge();
      }
    } //end of fill truth vars


    ///////////////////////////////////////
    // Fill vars only for tight leptons  //
    ///////////////////////////////////////

     if(lepton_isTight) {

       event->intVariable("Tlepton_" + std::to_string(nTight) + "_truthClassificationSM4t_bkgFlag") =
           event->intVariable("lepton_" + std::to_string(i) + "_truthClassificationSM4t_bkgFlag");

       event->intVariable("Tlepton_" + std::to_string(nTight) + "_ID") =event->intVariable("lepton_" + std::to_string(i) + "_ID");
       event->floatVariable("Tlepton_" + std::to_string(nTight) + "_mll_atPV") = event->floatVariable("lepton_" + std::to_string(i) + "_mll_atPV");
       event->floatVariable("Tlepton_" + std::to_string(nTight) + "_mll_atConV") = event->floatVariable("lepton_" + std::to_string(i) + "_mll_atConV");
       event->floatVariable("Tlepton_" + std::to_string(nTight) + "_pT") = lepton->Pt();

       event->floatVariable("Tlepton_" + std::to_string(nTight) + "_delta_z0_sintheta") =
          (m_leptonType == leptonType::electron) ?  lepton->floatVariable("el_delta_z0_sintheta")
                                                 :  lepton->floatVariable("mu_delta_z0_sintheta");
       event->floatVariable("Tlepton_" + std::to_string(nTight) + "_d0sig") =
          (m_leptonType == leptonType::electron) ?  lepton->floatVariable("el_d0sig")
                                                 :  lepton->floatVariable("mu_d0sig");

       event->intVariable("Tlepton_" + std::to_string(nTight) + "_charge") = lepton->charge();

       if (m_leptonType == leptonType::electron && i<3) tight_electrons.push_back(leptonFullM); //for charge flip and loose events. Count the number of tight in the first 3 leptons

       if (i<3) nTightChannel++;
       nTight++;
     }


  }// end of all leptons loop



  //////////////////////////////////////
  // Saving event level variables:    //
  //////////////////////////////////////

  // Number of tight  leptons
  event->intVariable("nTight") = nTight;


  // total charge: only tight (first 3 leptons)
  event->intVariable("total_charge") = total_charge;


  // For ttbarCO: el_MeePVCO
  event->floatVariable("el_MeePVCO") = GetMllPVCO(event->floatVariable("lepton_0_mll_atPV"),event->floatVariable("lepton_1_mll_atPV"));
  event->floatVariable("Tel_MeePVCO") = GetMllPVCO(event->floatVariable("Tlepton_0_mll_atPV"),event->floatVariable("Tlepton_1_mll_atPV"));

  // adding channel variable
  int channel = int(event->intVariable("SSee") == 1) * 1 + int(event->intVariable("SSem") == 1) * 2 +
                int(event->intVariable("OSee") == 1) * 1 + int(event->intVariable("OSem") == 1) * 2 +   //for OS, events must be counted within the SS category, for
                int(event->intVariable("SSmm") == 1) * 3 + int(event->intVariable("eee") == 1) * 4 +    // those with charge flip weight !=0
                int(event->intVariable("eem") == 1) * 5 + int(event->intVariable("emm") == 1) * 6 +
                int(event->intVariable("mmm") == 1) * 7 +
                int(event->intVariable("loose_SSee")==1) * 1 + int(event->intVariable("loose_SSem")==1) * 2 +
                int(event->intVariable("loose_SSmm")==1) * 3 + int(event->intVariable("loose_eee")==1) * 4 +
                int(event->intVariable("loose_eem")==1) * 5 + int(event->intVariable("loose_emm")==1) * 6 +
                int(event->intVariable("loose_mmm")==1) * 7;


  event->intVariable("Evt_Channel") = channel;

  // Event_BkgCategory: only tight
  if (Lepton_flag.size() == 2 && channel >= 1 && channel < 4) {
    SM4t::TruthHelpers::SM4tBkgCategory bkgCat =
          m_truthHelper->leptonBkgCategorisation(channel, Lepton_flag.at(0), Lepton_flag.at(1),
                                                 SM4t::TruthHelpers::SM4tTruthCategoryFlag::Other);
    event->intVariable("event_BkgCategory") = bkgCat;
  }
  else if (Lepton_flag.size() >= 3 && channel >= 4) {
    SM4t::TruthHelpers::SM4tBkgCategory bkgCat = m_truthHelper->leptonBkgCategorisation(
          channel, Lepton_flag.at(0), Lepton_flag.at(1), Lepton_flag.at(2));
    event->intVariable("event_BkgCategory") = bkgCat;
  }


  // Angular variables (can include loose leptons)
  event->floatVariable("deltaR_ll_max") = deltaR_ll_max;
  event->floatVariable("deltaR_ll_min") = deltaR_ll_min;
  event->floatVariable("deltaR_ll_sum") = deltaR_ll_sum;
  event->floatVariable("deltaR_lj_min") = deltaR_lj_min;
  event->floatVariable("deltaR_lb_min") = deltaR_lb_min;
  event->floatVariable("deltaR_lb_max") = deltaR_lb_max;
  event->floatVariable("m_min_deltaR_lj") = m_min_deltaR_lj;
  event->floatVariable("m_min_deltaR_lb") = m_min_deltaR_lb;


  // Angular vars computed using only tight leptons:
  event->floatVariable("TdeltaR_ll_max") = tight_deltaR_ll_max;
  event->floatVariable("TdeltaR_ll_min") = tight_deltaR_ll_min;
  event->floatVariable("TdeltaR_ll_sum") = tight_deltaR_ll_sum;
  event->floatVariable("TdeltaR_lj_min") = tight_deltaR_lj_min;
  event->floatVariable("TdeltaR_lb_min") = tight_deltaR_lb_min;
  event->floatVariable("TdeltaR_lb_max") = tight_deltaR_lb_max;
  event->floatVariable("Tm_min_deltaR_lj") = tight_m_min_deltaR_lj;
  event->floatVariable("Tm_min_deltaR_lb") = tight_m_min_deltaR_lb;


  /// Add the N_OSSF : include loose
  event->intVariable("N_OSSF") = N_OSSF;  //include loose

  /// Add event topology variables: include loose
  event->floatVariable("dR_LJ_all") = leptons_vectorsum.DeltaR(jets_vectorsum);
  event->floatVariable("All_lep_jet_mass") = (leptons_vectorsum+jets_vectorsum).M();
  event->floatVariable("All_lep_jet_pt") = (leptons_vectorsum+jets_vectorsum).Pt();
  event->floatVariable("All_lep_jet_eta") = (leptons_vectorsum+jets_vectorsum).Eta();
  event->floatVariable("All_lep_jet_phi") = (leptons_vectorsum+jets_vectorsum).Phi();
  event->floatVariable("All_lep_jet_ht")=(leptons_sum_pt+jets_sum_pt);
  event->floatVariable("All_lep_jet_pt_ht_ratio")=(leptons_vectorsum+jets_vectorsum).Pt()/(leptons_sum_pt+jets_sum_pt);
  event->floatVariable("All_lep_bjet_mass")=(leptons_vectorsum+ bjets_vectorsum).M();
  event->floatVariable("All_lep_bjet_ht")=(leptons_sum_pt+ bjets_sum_pt);
  event->floatVariable("All_lep_bjet_pt_ht_ratio")=(leptons_vectorsum+ bjets_vectorsum).Pt()/(leptons_sum_pt+ bjets_sum_pt);
  event->floatVariable("All_lep_bjet_pt")=(leptons_vectorsum+ bjets_vectorsum).Pt();
  event->floatVariable("All_lep_bjet_eta")=(leptons_vectorsum+ bjets_vectorsum).Eta();
  event->floatVariable("All_lep_bjet_phi")=(leptons_vectorsum+ bjets_vectorsum).Phi();


  ///// Use leading lepton and MET to calculate MTW
  if (leptons_n > 0) {
    lepton = event->m_leptons.at(0);
    float met_met = event->met_met;
    float met_phi = event->met_phi;
    event->floatVariable("mtw") = GetMTW(lepton,met_met,met_phi);
  }
  else event->floatVariable("mtw") = 0.0;


  // Saving Mll* vars: include loose
  event->floatVariable("Mll01") = 0;
  event->floatVariable("Mlll") = 0;
  event->floatVariable("M_OSSF_Z") = 0;

  float Mll01(0), Mll02(0), Mll12(0);
  float M_OSSF_Z(0);

  if (leptons_n >= 2) {
    event->floatVariable("Mll01") = GetMll(all_leptons[0],all_leptons[1]);
    if (leptons_n >= 3) {

      event->floatVariable("Mlll") = (all_leptons[0] + all_leptons[1] + all_leptons[2]).M();

      if (leptons_charge[0] * leptons_charge[1] < 0 && event_leptonType[0] == event_leptonType[1])
          Mll01 = GetMll(all_leptons[0], all_leptons[1]);

      if (leptons_charge[0] * leptons_charge[2] < 0 && event_leptonType[0] == event_leptonType[2])
          Mll02 = GetMll(all_leptons[0], all_leptons[2]);

      if (leptons_charge[1] * leptons_charge[2] < 0 && event_leptonType[1] == event_leptonType[2])
          Mll12 = GetMll(all_leptons[1], all_leptons[2]);


      if ( Mll01 > 0 && Mll02 == 0 && Mll12 == 0 ) M_OSSF_Z = Mll01;
      else if ( Mll01 == 0 && Mll02 > 0  && Mll12 == 0 ) M_OSSF_Z = Mll02;
      else if ( Mll01 == 0 && Mll02 == 0 && Mll12 > 0 ) M_OSSF_Z = Mll12;
      else if ( Mll01 > 0  && Mll02 > 0 && Mll12 == 0 ) M_OSSF_Z = GetMllClosestToZ(Mll01, Mll02);
      else if ( Mll01 > 0 && Mll02 == 0 && Mll12 > 0 )  M_OSSF_Z = GetMllClosestToZ(Mll01, Mll12);
      else if ( Mll01 == 0 && Mll02 > 0  && Mll12 > 0 ) M_OSSF_Z = GetMllClosestToZ(Mll02, Mll12);
      else if ( Mll01 > 0 && Mll02 > 0 && Mll12 > 0 ){

          float temp_Z = GetMllClosestToZ(Mll01, Mll02);
          M_OSSF_Z = GetMllClosestToZ(temp_Z, Mll12);

      }
    }
  }//end leptons to get Minv of pair closest to the Z

  event->floatVariable("M_OSSF_Z")=M_OSSF_Z;


  // Filling Z veto for 3L events: Add this variable if and only if we are working in the loose sample.
  if (event->intVariable("loose_eee")==1 || event->intVariable("loose_eem")==1 || event->intVariable("loose_emm")==1  || event->intVariable("loose_mmm")==1  ||
      event->intVariable("loose_SSee")==1|| event->intVariable("loose_SSem")==1|| event->intVariable("loose_SSmm")==1 ||
      event->intVariable("OSee")==1 || event->intVariable("OSem")==1 || event->intVariable("OSmm")==1) {

    if ( (event->intVariable("loose_eee")==1||event->intVariable("loose_eem")==1||event->intVariable("loose_emm")==1||event->intVariable("loose_mmm")==1 )
       && fabs(event->floatVariable("M_OSSF_Z") - 91000) <10000){
      if (event->intVariable("loose_eee")==1)  event->intVariable("loose_eee_Zveto") = 1;
      if (event->intVariable("loose_eem")==1)  event->intVariable("loose_eem_Zveto") = 1;
      if (event->intVariable("loose_emm")==1)  event->intVariable("loose_emm_Zveto") = 1;
      if (event->intVariable("loose_mmm")==1)  event->intVariable("loose_mmm_Zveto") = 1;
    }
    else {
      event->intVariable("loose_eee_Zveto") = 0;
      event->intVariable("loose_eem_Zveto") = 0;
      event->intVariable("loose_emm_Zveto") = 0;
      event->intVariable("loose_mmm_Zveto") = 0;
    }

    event->intVariable("SSee") = 0;
    event->intVariable("SSem") = 0;
    event->intVariable("SSmm") = 0;
    event->intVariable("eee") = 0;
    event->intVariable("eem") = 0;
    event->intVariable("emm") = 0;
    event->intVariable("mmm") = 0;

    // pass the loose event the tight criteria?
    if (nTightChannel == 2){
      if (event->intVariable("loose_SSee")==1)  event->intVariable("SSee") = 1;
      if (event->intVariable("loose_SSem")==1)  event->intVariable("SSem") = 1;
      if (event->intVariable("loose_SSmm")==1)  event->intVariable("SSmm") = 1;
      if (event->intVariable("loose_eee")==1)  event->intVariable("SSee") = 1;
      if (event->intVariable("loose_mmm")==1)  event->intVariable("SSmm") = 1;
      if (event->intVariable("loose_eem")==1){
          if (tight_electrons.size()==2)      event->intVariable("SSee") = 1;
          else if (tight_electrons.size()==1) event->intVariable("SSem") = 1;
      }
      if (event->intVariable("loose_emm")==1){
          if (tight_electrons.size()==1)      event->intVariable("SSem") = 1;
          else event->intVariable("SSmm") = 1;
      }

    }
    else if (nTightChannel ==3){
      if (event->intVariable("loose_eee")==1)  event->intVariable("eee") = 1;
      if (event->intVariable("loose_eem")==1)  event->intVariable("eem") = 1;
      if (event->intVariable("loose_emm")==1)  event->intVariable("emm") = 1;
      if (event->intVariable("loose_mmm")==1)  event->intVariable("mmm") = 1;
    }

  }//end of fill tight vars if loose sample




  //Adding charge flip weights
  if (doChargeFlip){

    if (event->intVariable("OSee")==1 || event->intVariable("OSem")==1 ){

      std::vector<double> rate(2,0);
      std::vector<double> rate_ttWCR(2,0);
      std::vector<double> rate_CRCO(2,0);

      std::vector<double> var_syst(2,0);
      std::vector<double> var_ttWCR_syst(2,0);
      std::vector<double> var_CRCO_syst(2,0);


      //std::cout << "- ##### Debug :" << std::endl;

      for (unsigned int k=0; k<tight_electrons.size(); k++){

        float t_el_mll_atPV=event->floatVariable("Tlepton_" + std::to_string(k) + "_mll_atPV");
        float t_el_eta = tight_electrons[k].Eta();
        float t_el_pt = tight_electrons[k].Pt()*0.001; //rates are function of pt in GeV (convert before passing to the function)

        if ( k < 2 ){
          rate[k] =       GetQmisIDLeptonRate(t_el_eta,t_el_pt, m_qmisid_def);
          rate_ttWCR[k] = GetQmisIDLeptonRate(t_el_eta,t_el_pt, m_qmisid_ttWCR);
          rate_CRCO[k]  = GetQmisIDLeptonRate(t_el_eta,t_el_pt, t_el_mll_atPV, m_qmisid_COCR_2eta);

          //be careful, those are not rates, but rather relative variations. Need to convert later to up and down rates
          var_syst[k] =       GetQmisIDLeptonRate(t_el_eta,t_el_pt, m_qmisid_def_syst);
          var_ttWCR_syst[k] = GetQmisIDLeptonRate(t_el_eta,t_el_pt, m_qmisid_ttWCR_syst);
          var_CRCO_syst[k] =  GetQmisIDLeptonRate(t_el_eta,t_el_pt, t_el_mll_atPV, m_qmisid_COCR_2eta_syst);
        }
        else std::cout << "--- Oopps!! You are counting more than 2 tight electrons in OS events... Something is wrong!" << std::endl;


        //std::cout << "- k : " << k << ",  eta = " <<  fabs(t_el_eta) << ", pt = " << t_el_pt << ", mll_atPV : " << t_el_mll_atPV << ",  rate_def = " << rate[k] <<  ", rate_CRttW  : " << rate_ttWCR[k] << ", rate_CRCO : " << rate_CRCO[k] <<", CO PT: " <<  rate_CRCO_pt[k] << std::endl;
      }// end loop over tight electrons

      double qMisID =      GetQmisIDWeight(rate[0], rate[1]);
      double qMisID_ttW =  GetQmisIDWeight(rate_ttWCR[0], rate_ttWCR[1]);
      double qMisID_CRCO_2eta = GetQmisIDWeight(rate_CRCO[0], rate_CRCO[1]);

       //std::cout << "### " << qMisID_CRCO_pt << std::endl;

      //getting up variations
      double qMisID_up =      GetQmisIDWeight((1+var_syst[0])*rate[0], (1+var_syst[1])*rate[1]);
      double qMisID_ttW_up =  GetQmisIDWeight((1+var_ttWCR_syst[0])*rate_ttWCR[0], (1+var_ttWCR_syst[1])*rate_ttWCR[1]);
      double qMisID_CRCO_up = GetQmisIDWeight((1+var_CRCO_syst[0])*rate_CRCO[0], (1+var_CRCO_syst[1])*rate_CRCO[1]); 

      //getting down variations
      double qMisID_down =      GetQmisIDWeight((1-var_syst[0])*rate[0], (1-var_syst[1])*rate[1]);
      double qMisID_ttW_down =  GetQmisIDWeight((1-var_ttWCR_syst[0])*rate_ttWCR[0], (1-var_ttWCR_syst[1])*rate_ttWCR[1]);
      double qMisID_CRCO_down = GetQmisIDWeight((1-var_CRCO_syst[0])*rate_CRCO[0], (1-var_CRCO_syst[1])*rate_CRCO[1]);
      
   

      event->floatVariable("charge_misID_weight") =  qMisID;
      event->floatVariable("charge_misID_weight_ttWCR") =  qMisID_ttW;
      event->floatVariable("charge_misID_weight_3D_COCR_2eta") =  qMisID_CRCO_2eta;
   

      event->floatVariable("charge_misID_weight_syst_up") =  qMisID_up;
      event->floatVariable("charge_misID_weight_3D_COCR_2eta_syst_up") =  qMisID_CRCO_up;
      event->floatVariable("charge_misID_weight_ttWCR_syst_up") =  qMisID_ttW_up;

      event->floatVariable("charge_misID_weight_syst_down") =  qMisID_down;
      event->floatVariable("charge_misID_weight_3D_COCR_2eta_syst_down") =  qMisID_CRCO_down;
      event->floatVariable("charge_misID_weight_ttWCR_syst_down") =  qMisID_ttW_down;


    }
    else {
      event->floatVariable("charge_misID_weight") =  0;
      event->floatVariable("charge_misID_weight_ttWCR") = 0;
      event->floatVariable("charge_misID_weight_3D_COCR_2eta") = 0;

      event->floatVariable("charge_misID_weight_syst_up") =  0;
      event->floatVariable("charge_misID_weight_3D_COCR_2eta_syst_up") = 0;
      event->floatVariable("charge_misID_weight_ttWCR_syst_up") = 0;

      event->floatVariable("charge_misID_weight_syst_down") =  0;
      event->floatVariable("charge_misID_weight_3D_COCR_2eta_syst_down") = 0;
      event->floatVariable("charge_misID_weight_ttWCR_syst_down") = 0;

    }


    // Better take out this to avoid confusing people
    // The following is a hack to "fake" OS events as SS ones (for *convenience* in the event selection when working with soft
    // code to  avoid  applying a different selection for the charge flip sample):
    // i.e. OSee -> SSee and OSem -> SSem. This way real SSee and SSem will become false (to avoid double counting)!
    // For completeness, set also 3L channels to 0 as they don't contribute to the charge flip sample. 

    //First reset to 0 real SS events
    event->intVariable("SSee") = 0;
    event->intVariable("SSee_passEtaCut") = 0;
    event->intVariable("SSee_passECIDS") = 0;
    event->intVariable("SSem") = 0;
    event->intVariable("SSem_passEtaCut") = 0;
    event->intVariable("SSem_passECIDS") = 0;
    event->intVariable("SSmm") = 0;
    event->intVariable("eee") =0;
    event->intVariable("eem") =0;
    event->intVariable("emm") =0;
    event->intVariable("mmm") =0;
    event->intVariable("eee_Zveto") =0;
    event->intVariable("eem_Zveto") =0;
    event->intVariable("emm_Zveto") =0;
    event->intVariable("mmm_Zveto") =0;



    //Then set OS as SS
    if (event->intVariable("OSee")==1){
        event->intVariable("SSee") = 1;
        if (event->intVariable("OSee_passEtaCut")==1) event->intVariable("SSee_passEtaCut") = 1;
        if (event->intVariable("OSee_passECIDS")==1) event->intVariable("SSee_passECIDS") = 1;
    }
    else if (event->intVariable("OSem")==1){
        event->intVariable("SSem") = 1;
        if (event->intVariable("OSem_passEtaCut")==1) event->intVariable("SSem_passEtaCut") = 1;
        if (event->intVariable("OSem_passECIDS")==1) event->intVariable("SSem_passECIDS") = 1;

    }

  }//end of add charge flip weights

  ///////=====Different combinations of leptons and b jets================////////////
  ////Minimization of sum_i mlb_i^2; Mlb_0 largest lb invariant mass squared after minimization

  /*
  event->floatVariable("Mlb_0") = 0;
  event->floatVariable("Mlb_1") = 0;
  event->floatVariable("Mlb_2") = 0;

  if (leptons_n >= 2 && event->intVariable("nBTags_DL1r_77") >= 2) {
    float m_lb_0 = 0;
    float m_lb_1 = 0;
    float m_lb_2 = 0;
    // CalculateMlb(all_leptons, all_bjets, m_lb_0, m_lb_1, m_lb_2);
    event->floatVariable("Mlb_0") = sqrt(m_lb_0);
    event->floatVariable("Mlb_1") = sqrt(m_lb_1);
    event->floatVariable("Mlb_2") = sqrt(m_lb_2);
  }*/



} // end AddLeptonVars


/* ---------------------------  >
---  Add Sphericity variable  ---
<  --------------------------- */

void AddSSMLVar::Sphericity(TTHbb::Event* event)
{
  std::shared_ptr<Jet> jet;
 std::shared_ptr<Lepton> lepton;
  unsigned int jets_n = event->m_jets.size();
  unsigned int leps_n = event->m_leptons.size();
  Eigen::Matrix2f SphericityXYJets;
  Eigen::Matrix2f SphericityXYJetsLepton;
  Eigen::Matrix3f SphericityJets;
  Eigen::Matrix3f SphericityJetslepton;
  Eigen::Matrix3f Sphericitylepton;
  float sum_jet_pt(0);
  float sum_jet_pXY(0);
  float sum_jet_lep_pXY(0);
  float sum_lep_pt(0);
  float sum_jet_lep_pt(0);
  //int njet = 0;
  SphericityXYJets <<0.0,0.0,
                     0.0,0.0;
  SphericityXYJetsLepton <<0.0,0.0,
                     0.0,0.0;
  SphericityJets << 0.0,0.0,0.0,
                    0.0,0.0,0.0,
                    0.0,0.0,0.0;
  SphericityJetslepton << 0.0,0.0,0.0,
                    0.0,0.0,0.0,
                    0.0,0.0,0.0;
  Sphericitylepton << 0.0,0.0,0.0,
                    0.0,0.0,0.0,
                    0.0,0.0,0.0;
  for (unsigned int i_jet = 0; i_jet <jets_n; i_jet ++) {
    jet = event->m_jets.at(i_jet);
    sum_jet_pt += jet->p4().Px()*jet->p4().Px()+jet->p4().Py()*jet->p4().Py()+jet->p4().Pz()*jet->p4().Pz();
    sum_jet_lep_pt +=  jet->p4().Px()*jet->p4().Px()+jet->p4().Py()*jet->p4().Py()+jet->p4().Pz()*jet->p4().Pz();
    sum_jet_pXY += jet->p4().Px()*jet->p4().Px()+jet->p4().Py()*jet->p4().Py();
    sum_jet_lep_pXY += jet->p4().Px()*jet->p4().Px()+jet->p4().Py()*jet->p4().Py();
    SphericityXYJets(0,0) += jet->p4().Px()*jet->p4().Px();
    SphericityXYJets(0,1) += jet->p4().Px()*jet->p4().Py();
    SphericityXYJets(1,0) += jet->p4().Py()*jet->p4().Px();
    SphericityXYJets(1,1) += jet->p4().Py()*jet->p4().Py();
    SphericityXYJetsLepton(0,0) += jet->p4().Px()*jet->p4().Px();
    SphericityXYJetsLepton(0,1) += jet->p4().Px()*jet->p4().Py();
    SphericityXYJetsLepton(1,0) += jet->p4().Py()*jet->p4().Px();
    SphericityXYJetsLepton(1,1) += jet->p4().Py()*jet->p4().Py();

    SphericityJets(0,0) += jet->p4().Px()*jet->p4().Px();
    SphericityJets(0,1) += jet->p4().Px()*jet->p4().Py();
    SphericityJets(0,2) += jet->p4().Px()*jet->p4().Pz();
    SphericityJets(1,0) += jet->p4().Py()*jet->p4().Px();
    SphericityJets(1,1) += jet->p4().Py()*jet->p4().Py();
    SphericityJets(1,2) += jet->p4().Py()*jet->p4().Pz();
    SphericityJets(2,0) += jet->p4().Pz()*jet->p4().Px();
    SphericityJets(2,1) += jet->p4().Pz()*jet->p4().Py();
    SphericityJets(2,2) += jet->p4().Pz()*jet->p4().Pz();

    SphericityJetslepton(0,0) += jet->p4().Px()*jet->p4().Px();
    SphericityJetslepton(0,1) += jet->p4().Px()*jet->p4().Py();
    SphericityJetslepton(0,2) += jet->p4().Px()*jet->p4().Pz();
    SphericityJetslepton(1,0) += jet->p4().Py()*jet->p4().Px();
    SphericityJetslepton(1,1) += jet->p4().Py()*jet->p4().Py();
    SphericityJetslepton(1,2) += jet->p4().Py()*jet->p4().Pz();
    SphericityJetslepton(2,0) += jet->p4().Pz()*jet->p4().Px();
    SphericityJetslepton(2,1) += jet->p4().Pz()*jet->p4().Py();
    SphericityJetslepton(2,2) += jet->p4().Pz()*jet->p4().Pz();
    }

  for (unsigned int i_lep = 0; i_lep <leps_n; i_lep ++) {
        lepton = event->m_leptons.at(i_lep);
    sum_lep_pt += lepton->p4().Px()*lepton->p4().Px()+lepton->p4().Py()*lepton->p4().Py()+lepton->p4().Pz()*lepton->p4().Pz();
    sum_jet_lep_pt += lepton->p4().Px()*lepton->p4().Px()+lepton->p4().Py()*lepton->p4().Py()+lepton->p4().Pz()*lepton->p4().Pz();
    sum_jet_lep_pXY += lepton->p4().Px()*lepton->p4().Px()+lepton->p4().Py()*lepton->p4().Py();
    SphericityXYJetsLepton(0,0) += lepton->p4().Px()*lepton->p4().Px();
    SphericityXYJetsLepton(0,1) += lepton->p4().Px()*lepton->p4().Py();
    SphericityXYJetsLepton(1,0) += lepton->p4().Py()*lepton->p4().Px();
    SphericityXYJetsLepton(1,1) += lepton->p4().Py()*lepton->p4().Py();

    SphericityJetslepton(0,0) += lepton->p4().Px()*lepton->p4().Px();
    SphericityJetslepton(0,1) += lepton->p4().Px()*lepton->p4().Py();
    SphericityJetslepton(0,2) += lepton->p4().Px()*lepton->p4().Pz();
    SphericityJetslepton(1,0) += lepton->p4().Py()*lepton->p4().Px();
    SphericityJetslepton(1,1) += lepton->p4().Py()*lepton->p4().Py();
    SphericityJetslepton(1,2) += lepton->p4().Py()*lepton->p4().Pz();
    SphericityJetslepton(2,0) += lepton->p4().Pz()*lepton->p4().Px();
    SphericityJetslepton(2,1) += lepton->p4().Pz()*lepton->p4().Py();
    SphericityJetslepton(2,2) += lepton->p4().Pz()*lepton->p4().Pz();

    Sphericitylepton(0,0) += lepton->p4().Px()*lepton->p4().Px();
    Sphericitylepton(0,1) += lepton->p4().Px()*lepton->p4().Py();
    Sphericitylepton(0,2) += lepton->p4().Px()*lepton->p4().Pz();
    Sphericitylepton(1,0) += lepton->p4().Py()*lepton->p4().Px();
    Sphericitylepton(1,1) += lepton->p4().Py()*lepton->p4().Py();
    Sphericitylepton(1,2) += lepton->p4().Py()*lepton->p4().Pz();
    Sphericitylepton(2,0) += lepton->p4().Pz()*lepton->p4().Px();
    Sphericitylepton(2,1) += lepton->p4().Pz()*lepton->p4().Py();
    Sphericitylepton(2,2) += lepton->p4().Pz()*lepton->p4().Pz();
  }

    SphericityXYJets(0,0) /= sum_jet_pXY;
    SphericityXYJets(0,1) /= sum_jet_pXY;
    SphericityXYJets(1,0) /= sum_jet_pXY;
    SphericityXYJets(1,1) /= sum_jet_pXY;
    SphericityXYJetsLepton(0,0) /= sum_jet_lep_pXY;
    SphericityXYJetsLepton(0,1) /= sum_jet_lep_pXY;
    SphericityXYJetsLepton(1,0) /= sum_jet_lep_pXY;
    SphericityXYJetsLepton(1,1) /= sum_jet_lep_pXY;

    SphericityJets(0,0) /= sum_jet_pt;
    SphericityJets(0,1) /= sum_jet_pt;
    SphericityJets(0,2) /= sum_jet_pt;
    SphericityJets(1,0) /= sum_jet_pt;
    SphericityJets(1,1) /= sum_jet_pt;
    SphericityJets(1,2) /= sum_jet_pt;
    SphericityJets(2,0) /= sum_jet_pt;
    SphericityJets(2,1) /= sum_jet_pt;
    SphericityJets(2,2) /= sum_jet_pt;

    SphericityJetslepton(0,0) /= sum_jet_lep_pt;
    SphericityJetslepton(0,1) /= sum_jet_lep_pt;
    SphericityJetslepton(0,2) /= sum_jet_lep_pt;
    SphericityJetslepton(1,0) /= sum_jet_lep_pt;
    SphericityJetslepton(1,1) /= sum_jet_lep_pt;
    SphericityJetslepton(1,2) /= sum_jet_lep_pt;
    SphericityJetslepton(2,0) /= sum_jet_lep_pt;
    SphericityJetslepton(2,1) /= sum_jet_lep_pt;
    SphericityJetslepton(2,2) /= sum_jet_lep_pt;

    Sphericitylepton(0,0) /= sum_lep_pt;
    Sphericitylepton(0,1) /= sum_lep_pt;
    Sphericitylepton(0,2) /= sum_lep_pt;
    Sphericitylepton(1,0) /= sum_lep_pt;
    Sphericitylepton(1,1) /= sum_lep_pt;
    Sphericitylepton(1,2) /= sum_lep_pt;
    Sphericitylepton(2,0) /= sum_lep_pt;
    Sphericitylepton(2,1) /= sum_lep_pt;
    Sphericitylepton(2,2) /= sum_lep_pt;

    Eigen::EigenSolver<Eigen::Matrix2f> es2;
    es2.compute(SphericityXYJets,false);
    std::complex<double> SphericityXY_lambda_1 = es2.eigenvalues()[0];
    std::complex<double> SphericityXY_lambda_2 = es2.eigenvalues()[1];
    float lambdaXY_1, lambdaXY_2;
    if (SphericityXY_lambda_1.real()>SphericityXY_lambda_2.real())
   {
      lambdaXY_1=SphericityXY_lambda_1.real();
      lambdaXY_2=SphericityXY_lambda_2.real();
   }
   else {
      lambdaXY_1=SphericityXY_lambda_2.real();
      lambdaXY_2=SphericityXY_lambda_1.real();
   }

    event->floatVariable("SphericityXYJets")=2*lambdaXY_2/(lambdaXY_2+lambdaXY_1);

   es2.compute(SphericityXYJetsLepton,false);
    SphericityXY_lambda_1 = es2.eigenvalues()[0];
    SphericityXY_lambda_2 = es2.eigenvalues()[1];
    if (SphericityXY_lambda_1.real()>SphericityXY_lambda_2.real())
   {
      lambdaXY_1=SphericityXY_lambda_1.real();
      lambdaXY_2=SphericityXY_lambda_2.real();
   }
   else {
      lambdaXY_1=SphericityXY_lambda_2.real();
      lambdaXY_2=SphericityXY_lambda_1.real();
   }

    event->floatVariable("SphericityXYJetsLepton")=2*lambdaXY_2/(lambdaXY_2+lambdaXY_1);

    Eigen::EigenSolver<Eigen::Matrix3f> es;
   // std::cout << "SphericityJets matrix:" << std::endl<<SphericityJets<<std::endl << std::endl;
    es.compute(SphericityJets,false);
   // std::cout << "SphericityJets matrix eigenvalues:"  <<  std::endl << es.eigenvalues() << std::endl;
    //event SphericityJets
    std::complex<double> Sphericity_lambda_1 = es.eigenvalues()[0];
    std::complex<double> Sphericity_lambda_2 = es.eigenvalues()[1];
    std::complex<double> Sphericity_lambda_3 = es.eigenvalues()[2];
    float lambda_1, lambda_2, lambda_3;
    if (Sphericity_lambda_1.real()>= Sphericity_lambda_2.real() && Sphericity_lambda_1.real()>= Sphericity_lambda_3.real())
     {
         lambda_1 = Sphericity_lambda_1.real();
       if (Sphericity_lambda_2.real()>= Sphericity_lambda_3.real())
       {
          lambda_2 = Sphericity_lambda_2.real();
          lambda_3 = Sphericity_lambda_3.real();
       }
       else {
          lambda_2 = Sphericity_lambda_3.real();
          lambda_3 = Sphericity_lambda_2.real();
       }
     }
    else if (Sphericity_lambda_2.real()>= Sphericity_lambda_1.real() && Sphericity_lambda_2.real()>= Sphericity_lambda_3.real())
    {
         lambda_1 = Sphericity_lambda_2.real();
       if (Sphericity_lambda_1.real()>= Sphericity_lambda_3.real())
       {
          lambda_2 = Sphericity_lambda_1.real();
          lambda_3 = Sphericity_lambda_3.real();
       }
       else {
          lambda_2 = Sphericity_lambda_3.real();
          lambda_3 = Sphericity_lambda_1.real();
       }
     }
    else {
         lambda_1 = Sphericity_lambda_3.real();
       if (Sphericity_lambda_1.real()>= Sphericity_lambda_2.real())
       {
          lambda_2 = Sphericity_lambda_1.real();
          lambda_3 = Sphericity_lambda_2.real();
       }
       else {
          lambda_2 = Sphericity_lambda_2.real();
          lambda_3 = Sphericity_lambda_1.real();
       }
    }
    //std::cout << "lambda: " << lambda_1 << ", " << lambda_2 << "," << lambda_3 << std::endl;
    //check jets
    event->floatVariable("SphericityJets")=3./2.*(lambda_2+lambda_3);
    event->floatVariable("AplanarityJets")=3./2.*(lambda_3);
    event->floatVariable("CJets")=3.*(lambda_1*lambda_2+lambda_1*lambda_3+lambda_2*lambda_3);
    event->floatVariable("DJets")=27.*(lambda_1*lambda_2*lambda_3);


   // std::cout << "Sphericitylepton matrix:" << std::endl<<Sphericitylepton<<std::endl << std::endl;
    es.compute(Sphericitylepton,false);
   // std::cout << "Sphericitylepton matrix eigenvalues:"  <<  std::endl << es.eigenvalues() << std::endl;
    //event Sphericitylepton
    Sphericity_lambda_1 = es.eigenvalues()[0];
    Sphericity_lambda_2 = es.eigenvalues()[1];
    Sphericity_lambda_3 = es.eigenvalues()[2];
    if (Sphericity_lambda_1.real()>= Sphericity_lambda_2.real() && Sphericity_lambda_1.real()>= Sphericity_lambda_3.real())
     {
         lambda_1 = Sphericity_lambda_1.real();
       if (Sphericity_lambda_2.real()>= Sphericity_lambda_3.real())
       {
          lambda_2 = Sphericity_lambda_2.real();
          lambda_3 = Sphericity_lambda_3.real();
       }
       else {
          lambda_2 = Sphericity_lambda_3.real();
          lambda_3 = Sphericity_lambda_2.real();
       }
     }
    else if (Sphericity_lambda_2.real()>= Sphericity_lambda_1.real() && Sphericity_lambda_2.real()>= Sphericity_lambda_3.real())
    {
         lambda_1 = Sphericity_lambda_2.real();
       if (Sphericity_lambda_1.real()>= Sphericity_lambda_3.real())
       {
          lambda_2 = Sphericity_lambda_1.real();
          lambda_3 = Sphericity_lambda_3.real();
       }
       else {
          lambda_2 = Sphericity_lambda_3.real();
          lambda_3 = Sphericity_lambda_1.real();
       }
     }
    else {
         lambda_1 = Sphericity_lambda_3.real();
       if (Sphericity_lambda_1.real()>= Sphericity_lambda_2.real())
       {
          lambda_2 = Sphericity_lambda_1.real();
          lambda_3 = Sphericity_lambda_2.real();
       }
       else {
          lambda_2 = Sphericity_lambda_2.real();
          lambda_3 = Sphericity_lambda_1.real();
       }
    }
    event->floatVariable("SphericityLepton")=3./2.*(lambda_2+lambda_3);
    event->floatVariable("AplanarityLepton")=3./2.*(lambda_3);
    event->floatVariable("CLepton")=3.*(lambda_1*lambda_2+lambda_1*lambda_3+lambda_2*lambda_3);
    event->floatVariable("DLepton")=27.*(lambda_1*lambda_2*lambda_3);

   // std::cout << "SphericityJetslepton matrix:" << std::endl<<SphericityJetslepton<<std::endl << std::endl;
    es.compute(SphericityJetslepton,false);
   // std::cout << "SphericityJetslepton matrix eigenvalues:"  <<  std::endl << es.eigenvalues() << std::endl;
    //event SphericityJets
    Sphericity_lambda_1 = es.eigenvalues()[0];
    Sphericity_lambda_2 = es.eigenvalues()[1];
    Sphericity_lambda_3 = es.eigenvalues()[2];
    if (Sphericity_lambda_1.real()>= Sphericity_lambda_2.real() && Sphericity_lambda_1.real()>= Sphericity_lambda_3.real())
     {
         lambda_1 = Sphericity_lambda_1.real();
       if (Sphericity_lambda_2.real()>= Sphericity_lambda_3.real())
       {
          lambda_2 = Sphericity_lambda_2.real();
          lambda_3 = Sphericity_lambda_3.real();
       }
       else {
          lambda_2 = Sphericity_lambda_3.real();
          lambda_3 = Sphericity_lambda_2.real();
       }
     }
    else if (Sphericity_lambda_2.real()>= Sphericity_lambda_1.real() && Sphericity_lambda_2.real()>= Sphericity_lambda_3.real())
    {
         lambda_1 = Sphericity_lambda_2.real();
       if (Sphericity_lambda_1.real()>= Sphericity_lambda_3.real())
       {
          lambda_2 = Sphericity_lambda_1.real();
          lambda_3 = Sphericity_lambda_3.real();
       }
       else {
          lambda_2 = Sphericity_lambda_3.real();
          lambda_3 = Sphericity_lambda_1.real();
       }
     }
    else {
         lambda_1 = Sphericity_lambda_3.real();
       if (Sphericity_lambda_1.real()>= Sphericity_lambda_2.real())
       {
          lambda_2 = Sphericity_lambda_1.real();
          lambda_3 = Sphericity_lambda_2.real();
       }
       else {
          lambda_2 = Sphericity_lambda_2.real();
          lambda_3 = Sphericity_lambda_1.real();
       }
    }
    event->floatVariable("SphericityJetLepton")=3./2.*(lambda_2+lambda_3);
    event->floatVariable("AplanarityJetLepton")=3./2.*(lambda_3);
    event->floatVariable("CJetLepton")=3.*(lambda_1*lambda_2+lambda_1*lambda_3+lambda_2*lambda_3);
    event->floatVariable("DJetLepton")=27.*(lambda_1*lambda_2*lambda_3);
}


//==================================================================================================//
//========================================Add the Jet
// variables=====================================//
//==================================================================================================//


void AddSSMLVar::AddJetVariables(TTHbb::Event* event) {

  // Declare variables
  std::shared_ptr<Jet> jet;
  std::shared_ptr<Jet> jet1;

  std::vector<float> jets_pt;
  std::vector<float> bjets_pt;

  TLorentzVector AllJets(0,0,0,0);

  float leading_jet_pt(-999);
  float jet_lowest_pt(1e6);
  float leading_bjet_pt(-999);

  float deltaR_jj_min(100);
  float deltaR_bj_min(100);
  float deltaR_bb_min(100);

  float m_bb_min(10000000);
  float m_bj_min(10000000);

  float sum_dl1r(0);
  float sum_dl1(0);
  int sum_Continuous_dl1r(0);
  int sum_Continuous_dl1(0);
  std::vector<int> all_Continuous_dl1r;
  std::vector<int> all_Continuous_dl1;

  // metsai - Variables for jet_DL1r_sum
  float jet_DL1r_first(-100);
  float jet_DL1r_second(-100);
  float jet_DL1r_third(-100);
  float jet_DL1r_fourth(-100);
  float jet_DL1r_fifth(-100);
  float jet_DL1r_sixth(-100);

  float jet_DL1r_first_bin(-100);
  float jet_DL1r_second_bin(-100);
  float jet_DL1r_third_bin(-100);
  float jet_DL1r_fourth_bin(-100);
  float jet_DL1r_fifth_bin(-100);
  float jet_DL1r_sixth_bin(-100);
  // metsai - Variables for jet_DL1r_sum
   //Adding truth jets related variable for syst and this is checked only for all the jets zzheng
   int n_truthbjets_all = 0;
   int n_truthcjets_all = 0;
   int n_truthljets_all = 0;
   int n_truthbjets = 0;//only checked for btagged jets
   int n_truthcjets = 0;//only checked for btagged jets
   int n_truthljets = 0;//only checed for btagged jets

  unsigned int jets_n = event->m_jets.size();



  // Loop over jets
  for (unsigned int j = 0; j < jets_n; j++) {

    jet = event->m_jets.at(j);

    jets_pt.push_back(jet->Pt());

    if(!isData) {
        if ( jet->intVariable("jet_truthflav")==5) n_truthbjets_all++;
        if ( jet->intVariable("jet_truthflav")==4) n_truthcjets_all++;
        if ( jet->intVariable("jet_truthflav")==0) n_truthljets_all++;
     }
    if (jet->charVariable("jet_isbtagged_DL1r_77")) bjets_pt.push_back(jet->Pt());


    sum_dl1r += jet->floatVariable("jet_DL1r");
    sum_dl1 += jet->floatVariable("jet_DL1");

    all_Continuous_dl1r.push_back(jet->intVariable("jet_tagWeightBin_DL1r_Continuous"));
    all_Continuous_dl1.push_back(jet->intVariable("jet_tagWeightBin_DL1_Continuous"));

    sum_Continuous_dl1r += jet->intVariable("jet_tagWeightBin_DL1r_Continuous");
    sum_Continuous_dl1 += jet->intVariable("jet_tagWeightBin_DL1_Continuous");

    TLorentzVector jetFullM;
    jetFullM.SetPtEtaPhiE(jet->Pt(), jet->Eta(), jet->Phi(), jet->E());

    AllJets += jetFullM;

    if (leading_jet_pt < jet->Pt()) leading_jet_pt = jet->Pt();
    if (jet_lowest_pt > jet->Pt()) jet_lowest_pt = jet->Pt();


    if (jet->charVariable("jet_isbtagged_DL1r_77")) {
      if (leading_bjet_pt < jet->Pt()) leading_bjet_pt = jet->Pt();

      if(!isData) {//zzheng/metsai: check inside btagged jets, and this is further used as a variable to derived systematics
          if ( jet->intVariable("jet_truthflav")==5) n_truthbjets++;
          if ( jet->intVariable("jet_truthflav")==4) n_truthcjets++;
          if ( jet->intVariable("jet_truthflav")==0) n_truthljets++;
      }


    }

    // metsai - to include jet_sum_DL1_Continuous_leadingFour


    float jet_DL1r_temp =jet->floatVariable("jet_DL1r");
    float jet_DL1r_temp_Continuous = jet->intVariable("jet_tagWeightBin_DL1r_Continuous");
    if ( jet_DL1r_temp>jet_DL1r_first) {

       jet_DL1r_sixth = jet_DL1r_fifth;
       jet_DL1r_fifth = jet_DL1r_fourth;
       jet_DL1r_fourth = jet_DL1r_third;
       jet_DL1r_third = jet_DL1r_second;
       jet_DL1r_second = jet_DL1r_first;
       jet_DL1r_first = jet_DL1r_temp;

       jet_DL1r_sixth_bin = jet_DL1r_fifth_bin;
       jet_DL1r_fifth_bin = jet_DL1r_fourth_bin;
       jet_DL1r_fourth_bin = jet_DL1r_third_bin;
       jet_DL1r_third_bin = jet_DL1r_second_bin;
       jet_DL1r_second_bin = jet_DL1r_first_bin;
       jet_DL1r_first_bin = jet_DL1r_temp_Continuous;

    }
    else if ( jet_DL1r_temp>jet_DL1r_second){
      jet_DL1r_sixth = jet_DL1r_fifth;
      jet_DL1r_fifth = jet_DL1r_fourth;
      jet_DL1r_fourth = jet_DL1r_third;
      jet_DL1r_third = jet_DL1r_second;
      jet_DL1r_second = jet_DL1r_temp;

      jet_DL1r_sixth_bin = jet_DL1r_fifth_bin;
      jet_DL1r_fifth_bin = jet_DL1r_fourth_bin;
      jet_DL1r_fourth_bin = jet_DL1r_third_bin;
      jet_DL1r_third_bin = jet_DL1r_second_bin;
      jet_DL1r_second_bin = jet_DL1r_temp_Continuous;

    }
    else if (jet_DL1r_temp>jet_DL1r_third) {
      jet_DL1r_sixth = jet_DL1r_fifth;
      jet_DL1r_fifth = jet_DL1r_fourth;
      jet_DL1r_fourth = jet_DL1r_third;
      jet_DL1r_third = jet_DL1r_temp;

      jet_DL1r_sixth_bin = jet_DL1r_fifth_bin;
      jet_DL1r_fifth_bin = jet_DL1r_fourth_bin;
      jet_DL1r_fourth_bin = jet_DL1r_third_bin;
      jet_DL1r_third_bin = jet_DL1r_temp_Continuous;
    }
    else if  (jet_DL1r_temp>jet_DL1r_fourth) {
      jet_DL1r_sixth = jet_DL1r_fifth;
      jet_DL1r_fifth = jet_DL1r_fourth;
      jet_DL1r_fourth =jet_DL1r_temp;

      jet_DL1r_sixth_bin = jet_DL1r_fifth_bin;
      jet_DL1r_fifth_bin = jet_DL1r_fourth_bin;
      jet_DL1r_fourth_bin = jet_DL1r_temp_Continuous;
    }
    else if  (jet_DL1r_temp>jet_DL1r_fifth) {
      jet_DL1r_sixth = jet_DL1r_fifth;
      jet_DL1r_fifth = jet_DL1r_temp;

      jet_DL1r_sixth_bin = jet_DL1r_fifth_bin;
      jet_DL1r_fifth_bin = jet_DL1r_temp_Continuous;
    }
    else if  (jet_DL1r_temp>jet_DL1r_sixth) {
      jet_DL1r_sixth = jet_DL1r_temp;
      jet_DL1r_sixth_bin = jet_DL1r_temp_Continuous;
    }


    // metsai - to include jet_sum_DL1_Continuous_leadingFour






    // Get deltaR wrt other jets
    for (unsigned int k = j + 1; k < jets_n && jets_n > 1; k++) {

      jet1 = event->m_jets.at(k);

      TLorentzVector jetFullM_other;
      jetFullM_other.SetPtEtaPhiE(jet1->Pt(), jet1->Eta(), jet1->Phi(), jet1->E());

      float deltaR_jj_temp(100);
      float m_jj_temp(1000000);

      deltaR_jj_temp = jetFullM_other.DeltaR(jetFullM);
      m_jj_temp = (jetFullM_other+jetFullM).M();

      if (deltaR_jj_min > deltaR_jj_temp) deltaR_jj_min = deltaR_jj_temp;

      if (jet->charVariable("jet_isbtagged_DL1r_77") || jet1->charVariable("jet_isbtagged_DL1r_77")) {

        if (deltaR_bj_min > deltaR_jj_temp) deltaR_bj_min = deltaR_jj_temp;
        if (m_bj_min > m_jj_temp) m_bj_min = m_jj_temp;

        if (jet->charVariable("jet_isbtagged_DL1r_77") && jet1->charVariable("jet_isbtagged_DL1r_77")){
             if (deltaR_bb_min > deltaR_jj_temp ) deltaR_bb_min = deltaR_jj_temp; // seems to be buggy at the original code. Corrected here.
             if (m_bb_min > m_jj_temp) m_bb_min = m_jj_temp;
        }//both b-tag

      }// at least 1 b-tag
     }// end loop other jets


  }// End loop over jets


  // Saving event variables
  event->floatVariable("jet_sum_DL1r") = sum_dl1r;
  event->floatVariable("jet_sum_DL1") = sum_dl1;
  event->intVariable("jet_sum_DL1r_Continuous") = sum_Continuous_dl1r;
  event->intVariable("jet_sum_DL1_Continuous") = sum_Continuous_dl1;

  event->floatVariable("jet_sum_DL1r_Continuous_leadingTwo")   =  jet_DL1r_first_bin+jet_DL1r_second_bin;
  event->floatVariable("jet_sum_DL1r_Continuous_leadingThree") =  jet_DL1r_first_bin+jet_DL1r_second_bin+jet_DL1r_third_bin;
  event->floatVariable("jet_sum_DL1r_Continuous_leadingFour")  =  jet_DL1r_first_bin+jet_DL1r_second_bin+jet_DL1r_third_bin+jet_DL1r_fourth_bin;
  event->floatVariable("jet_sum_DL1r_Continuous_leadingFive")  =  jet_DL1r_first_bin+jet_DL1r_second_bin+jet_DL1r_third_bin+jet_DL1r_fourth_bin+jet_DL1r_fifth_bin;
  event->floatVariable("jet_sum_DL1r_Continuous_leadingSix")   =  jet_DL1r_first_bin+jet_DL1r_second_bin+jet_DL1r_third_bin+jet_DL1r_fourth_bin+jet_DL1r_fifth_bin+jet_DL1r_sixth_bin;


  event->floatVariable("jet_sum_mass") = AllJets.M();
  event->floatVariable("jet_sum_pt") = AllJets.Pt();
  event->floatVariable("jet_sum_eta") = AllJets.Eta();
  event->floatVariable("jet_sum_phi") = AllJets.Phi();


  std::sort(all_Continuous_dl1r.begin(), all_Continuous_dl1r.end());
  std::sort(all_Continuous_dl1.begin(), all_Continuous_dl1.end());
  for (unsigned int i(0); i < jets_n; i++) {
    event->intVariable("jet_DL1r_Continuous_order_" + std::to_string(i)) =  all_Continuous_dl1r.at(i);
    event->intVariable("jet_DL1_Continuous_order_" + std::to_string(i))    = all_Continuous_dl1.at(i);
  }

  event->floatVariable("leading_jet_pT") = leading_jet_pt;

  std::sort(jets_pt.begin(), jets_pt.end(),std::greater<float>());
  std::sort(bjets_pt.begin(), bjets_pt.end(),std::greater<float>());
  //std::cout << "nJets = " << jets_pt.size() << std::endl;
  for (unsigned int i(1); i < 6; i++) {
    if (i<jets_pt.size()){
      event->floatVariable("jet_" + std::to_string(i)+"_pt") = jets_pt.at(i);
      //std::cout << "jet_" + std::to_string(i)+"_pt = " << jets_pt.at(i) << std::endl;
    }
    else{
      event->floatVariable("jet_" + std::to_string(i)+"_pt") = -999;
      //std::cout << "jet_" + std::to_string(i)+"_pt = " << -999 << std::endl;
    }
  }

  event->floatVariable("bjet_pt_1") = -999;
  if (bjets_pt.size()>=2) event->floatVariable("bjet_pt_1") = bjets_pt.at(1);


  event->floatVariable("lowest_jet_pT") = jet_lowest_pt;
  event->floatVariable("leading_bjet_pT") = leading_bjet_pt;

  event->floatVariable("deltaR_bb_min") = deltaR_bb_min;
  event->floatVariable("deltaR_bj_min") = deltaR_bj_min;
  event->floatVariable("m_bb_min") = m_bb_min;
  event->floatVariable("m_bj_min") = m_bj_min;
  event->floatVariable("deltaR_jj_min") = deltaR_jj_min;
  event->intVariable("truthbjets_b77jets") = n_truthbjets;
  event->intVariable("truthcjets_b77jets") = n_truthcjets;
  event->intVariable("truthljets_b77jets") = n_truthljets;
  event->intVariable("truthbjets_all") = n_truthbjets_all;
  event->intVariable("truthcjets_all") = n_truthcjets_all;
  event->intVariable("truthljets_all") = n_truthljets_all;

  event->floatVariable("HT_jets_noleadjet") = event->floatVariable("HT_jets") - leading_jet_pt;

  return;
}

// Lennart: Why is there a test in the branch supposed to be merged into master?
// Bad coding, just a test
void AddSSMLVar::CalculateMlb(std::vector<TLorentzVector> leptons,
                              std::vector<TLorentzVector> bjets, float& m_lb_0, float& m_lb_1,
                              float& m_lb_2) {
  unsigned int leptons_n = leptons.size();
  unsigned int bjets_n = bjets.size();

  if (leptons_n < 2 || bjets_n < 2) return;

  float M_l0b0(0), M_l1b1(0), M_l0b1(0), M_l1b0(0);
  M_l0b0 = (leptons[0] + bjets[0]).M2();
  M_l1b1 = (leptons[1] + bjets[1]).M2();
  M_l0b1 = (leptons[0] + bjets[1]).M2();
  M_l1b0 = (leptons[1] + bjets[0]).M2();

  if (leptons_n == 2 && bjets_n == 2) {
    // combination_1 l0b0,l1b1;
    float combination_1 = M_l0b0 + M_l1b1;
    // combination_2 l0b1,l1b0;
    float combination_2 = M_l0b1 + M_l1b0;

    if (combination_1 < combination_2) {
      // mlb is the invariant mass between lepton and b jet
      m_lb_0 = M_l0b0 > M_l1b1 ? M_l0b0 : M_l1b1;
      m_lb_1 = M_l0b0 < M_l1b1 ? M_l0b0 : M_l1b1;
    } else {
      m_lb_0 = M_l0b1 > M_l1b0 ? M_l0b1 : M_l1b0;
      m_lb_1 = M_l0b1 < M_l1b0 ? M_l0b1 : M_l1b0;
    }
  }

  if (leptons_n == 2 && bjets_n >= 3) {
    float M_l0b2(0), M_l1b2(0);
    M_l0b2 = (leptons[0] + bjets[2]).M2();
    M_l1b2 = (leptons[1] + bjets[2]).M2();
    std::vector<float> combination;
    combination.push_back(M_l0b0 + M_l1b1);
    combination.push_back(M_l0b0 + M_l1b2);
    combination.push_back(M_l0b1 + M_l1b0);
    combination.push_back(M_l0b1 + M_l1b2);
    combination.push_back(M_l0b2 + M_l1b0);
    combination.push_back(M_l0b2 + M_l1b1);

    std::sort(combination.begin(), combination.end());
    if (fabs(combination[0] - (M_l0b0 + M_l1b1)) < 1e-3) {
      m_lb_0 = M_l0b0 > M_l1b1 ? M_l0b0 : M_l1b1;
      m_lb_1 = M_l0b0 < M_l1b1 ? M_l0b0 : M_l1b1;
    }

    if (fabs(combination[0] - (M_l0b0 + M_l1b2)) < 1e-3) {
      m_lb_0 = M_l0b0 > M_l1b2 ? M_l0b0 : M_l1b2;
      m_lb_1 = M_l0b0 < M_l1b2 ? M_l0b0 : M_l1b2;
    }

    if (fabs(combination[0] - (M_l0b1 + M_l1b0)) < 1e-3) {
      m_lb_0 = M_l0b1 > M_l1b0 ? M_l0b1 : M_l1b0;
      m_lb_1 = M_l0b1 < M_l1b0 ? M_l0b1 : M_l1b0;
    }

    if (fabs(combination[0] - (M_l0b1 + M_l1b2)) < 1e-3) {
      m_lb_0 = M_l0b1 > M_l1b2 ? M_l0b1 : M_l1b2;
      m_lb_1 = M_l0b1 < M_l1b2 ? M_l0b1 : M_l1b2;
    }
    if (fabs(combination[0] - (M_l0b2 + M_l1b0)) < 1e-3) {
      m_lb_0 = M_l0b2 > M_l1b0 ? M_l0b2 : M_l1b0;
      m_lb_1 = M_l0b2 < M_l1b0 ? M_l0b2 : M_l1b0;
    }
    if (fabs(combination[0] - (M_l0b2 + M_l1b1)) < 1e-3) {
      m_lb_0 = M_l0b2 > M_l1b1 ? M_l0b2 : M_l1b1;
      m_lb_1 = M_l0b2 < M_l1b1 ? M_l0b2 : M_l1b1;
    }
  }

  if (leptons_n >= 3 && bjets_n == 2) {
    float M_l2b0(0), M_l2b1(0);
    M_l2b0 = (leptons[2] + bjets[0]).M2();
    M_l2b1 = (leptons[2] + bjets[1]).M2();

    std::vector<float> combination;
    combination.push_back(M_l0b0 + M_l1b1);
    combination.push_back(M_l0b0 + M_l2b1);
    combination.push_back(M_l0b1 + M_l1b0);
    combination.push_back(M_l0b1 + M_l2b0);
    combination.push_back(M_l1b0 + M_l2b1);
    combination.push_back(M_l1b1 + M_l2b0);
    std::sort(combination.begin(), combination.end());
    if (fabs(combination[0] - (M_l0b0 + M_l1b1)) < 1e-3) {
      m_lb_0 = M_l0b0 > M_l1b1 ? M_l0b0 : M_l1b1;
      m_lb_1 = M_l0b0 < M_l1b1 ? M_l0b0 : M_l1b1;
    }
    if (fabs(combination[0] - (M_l0b0 + M_l2b1)) < 1e-3) {
      m_lb_0 = M_l0b0 > M_l2b1 ? M_l0b0 : M_l2b1;
      m_lb_1 = M_l0b0 < M_l2b1 ? M_l0b0 : M_l2b1;
    }

    if (fabs(combination[0] - (M_l0b1 + M_l1b0)) < 1e-3) {
      m_lb_0 = M_l0b1 > M_l1b0 ? M_l0b1 : M_l1b0;
      m_lb_1 = M_l0b1 < M_l1b0 ? M_l0b1 : M_l1b0;
    }
    if (fabs(combination[0] - (M_l0b1 + M_l2b0)) < 1e-3) {
      m_lb_0 = M_l0b1 > M_l2b0 ? M_l0b1 : M_l2b0;
      m_lb_1 = M_l0b1 < M_l2b0 ? M_l0b1 : M_l2b0;
    }

    if (fabs(combination[0] - (M_l1b0 + M_l2b1)) < 1e-3) {
      m_lb_0 = M_l1b0 > M_l2b1 ? M_l1b0 : M_l2b1;
      m_lb_1 = M_l1b0 < M_l2b1 ? M_l1b0 : M_l2b1;
    }
    if (fabs(combination[0] - (M_l1b1 + M_l2b0)) < 1e-3) {
      m_lb_0 = M_l1b1 > M_l2b0 ? M_l1b1 : M_l2b0;
      m_lb_1 = M_l1b1 < M_l2b0 ? M_l1b1 : M_l2b0;
    }
  }
  if (leptons_n >= 3 && bjets_n >= 3) {
    float M_l2b0(0), M_l2b1(0);
    M_l2b0 = (leptons[2] + bjets[0]).M2();
    M_l2b1 = (leptons[2] + bjets[1]).M2();
    float M_l0b2(0), M_l1b2(0);
    M_l0b2 = (leptons[0] + bjets[2]).M2();
    M_l1b2 = (leptons[1] + bjets[2]).M2();
    float M_l2b2(0);
    M_l2b2 = (leptons[2] + bjets[2]).M2();

    std::vector<float> combination;
    combination.push_back(M_l0b0 + M_l1b1 + M_l2b2);
    combination.push_back(M_l0b0 + M_l1b2 + M_l2b1);
    combination.push_back(M_l0b1 + M_l1b0 + M_l2b2);
    combination.push_back(M_l0b1 + M_l1b2 + M_l2b0);
    combination.push_back(M_l0b2 + M_l1b1 + M_l2b0);
    combination.push_back(M_l0b2 + M_l1b0 + M_l2b1);
    if (fabs(combination[0] - (M_l0b0 + M_l1b1 + M_l2b2)) < 1e-3) {
      std::vector<float> temp_m;
      temp_m.push_back(M_l0b0);
      temp_m.push_back(M_l1b1);
      temp_m.push_back(M_l2b2);
      std::sort(temp_m.begin(), temp_m.end());
      m_lb_0 = temp_m[2];
      m_lb_1 = temp_m[1];
      m_lb_2 = temp_m[0];
    }
    if (fabs(combination[0] - (M_l0b0 + M_l1b2 + M_l2b1)) < 1e-3) {
      std::vector<float> temp_m;
      temp_m.push_back(M_l0b0);
      temp_m.push_back(M_l1b2);
      temp_m.push_back(M_l2b1);
      std::sort(temp_m.begin(), temp_m.end());
      m_lb_0 = temp_m[2];
      m_lb_1 = temp_m[1];
      m_lb_2 = temp_m[0];
    }

    if (fabs(combination[0] - (M_l0b1 + M_l1b0 + M_l2b2)) < 1e-3) {
      std::vector<float> temp_m;
      temp_m.push_back(M_l0b1);
      temp_m.push_back(M_l1b0);
      temp_m.push_back(M_l2b2);
      std::sort(temp_m.begin(), temp_m.end());
      m_lb_0 = temp_m[2];
      m_lb_1 = temp_m[1];
      m_lb_2 = temp_m[0];
    }
    if (fabs(combination[0] - (M_l0b1 + M_l1b2 + M_l2b0)) < 1e-3) {
      std::vector<float> temp_m;
      temp_m.push_back(M_l0b1);
      temp_m.push_back(M_l1b2);
      temp_m.push_back(M_l2b0);
      std::sort(temp_m.begin(), temp_m.end());
      m_lb_0 = temp_m[2];
      m_lb_1 = temp_m[1];
      m_lb_2 = temp_m[0];
    }

    if (fabs(combination[0] - (M_l0b2 + M_l1b1 + M_l2b0)) < 1e-3) {
      std::vector<float> temp_m;
      temp_m.push_back(M_l0b2);
      temp_m.push_back(M_l1b1);
      temp_m.push_back(M_l2b0);
      std::sort(temp_m.begin(), temp_m.end());
      m_lb_0 = temp_m[2];
      m_lb_1 = temp_m[1];
      m_lb_2 = temp_m[0];
    }
    if (fabs(combination[0] - (M_l0b2 + M_l1b0 + M_l2b1)) < 1e-3) {
      std::vector<float> temp_m;
      temp_m.push_back(M_l0b2);
      temp_m.push_back(M_l1b0);
      temp_m.push_back(M_l2b1);
      std::sort(temp_m.begin(), temp_m.end());
      m_lb_0 = temp_m[2];
      m_lb_1 = temp_m[1];
      m_lb_2 = temp_m[0];
    }
  }
}

double AddSSMLVar::GetQmisIDLeptonRate(float eta,float pt,float mll,TH3D *pH){

  //Original histo has the following axis: x,y,z -> eta,pt,mll

  //Check pt is within range
  if (pt > pH->GetYaxis()->GetBinUpEdge(pH->GetNbinsY())) pt = pH->GetYaxis()->GetBinCenter(pH->GetNbinsY());

  //Check mll is within range
  if (mll > pH->GetZaxis()->GetBinUpEdge(pH->GetNbinsZ())) mll = pH->GetZaxis()->GetBinCenter(pH->GetNbinsZ());
  else if (mll < pH->GetZaxis()->GetBinLowEdge(1)) mll = pH->GetZaxis()->GetBinCenter(1);


  int bin=pH->FindBin(fabs(eta),pt,mll);

  double epsilon=pH->GetBinContent(bin);

  if (epsilon < 0) epsilon = 0;
  else if (epsilon > 1 ) epsilon = 1;

  return epsilon;

}
double AddSSMLVar::GetQmisIDLeptonRate(float eta,float pt,TH2D *pH){

  //Check first that pT is within range. For very high pT, look in the last pT bin
  if (pt > pH->GetYaxis()->GetBinUpEdge(pH->GetNbinsY())) pt = pH->GetYaxis()->GetBinCenter(pH->GetNbinsY());
  if (pt < pH->GetYaxis()->GetBinLowEdge(1)) pt = pH->GetYaxis()->GetBinCenter(1);

  double epsilon=0;

  int bin=pH->FindBin(fabs(eta),pt);

  epsilon=pH->GetBinContent(bin);

  if (epsilon < 0) epsilon = 0;
  else if (epsilon > 1 ) epsilon = 1;


 return epsilon;
}
double AddSSMLVar::GetQmisIDWeight(double q1,double q2){


  double num = q1 + q2 - 2*q1*q2;
  double den = 1 - q1 -q2 + q1*q2;

  double weight = num/den;


  return weight;
}
bool AddSSMLVar::SetLTriggerMatching(std::shared_ptr<TTHbb::Lepton> lepton,int year){


  bool match=false;

  if (lepton->type() == leptonType::electron){

    match=(((year==15) &&(((lepton->Pt()/1000.>24)  && lepton->charVariable("el_trigMatch_HLT_e24_lhmedium_L1EM20VH"))
                        ||((lepton->Pt()/1000.>60)  && lepton->charVariable("el_trigMatch_HLT_e60_lhmedium"))
                        ||((lepton->Pt()/1000.>120) && lepton->charVariable("el_trigMatch_HLT_e120_lhloose"))))
         ||((year>=16||year<=18) && (((lepton->Pt()/1000.>60)  && lepton->charVariable("el_trigMatch_HLT_e60_lhmedium_nod0"))
                                  || ((lepton->Pt()/1000.>140) && lepton->charVariable("el_trigMatch_HLT_e140_lhloose_nod0"))))
         ||lepton->charVariable("el_isDLTMatch"));
  }
  else if (lepton->type() == leptonType::muon){
    match= (lepton->charVariable("mu_isDLTMatch") || ((lepton->Pt()/1000.>50)&&lepton->charVariable("mu_isSLTMatch")));
  }


 //if (match==true) std::cout << "-- mu_isDLTMatch : "<< lepton->charVariable("mu_isDLTMatch")   << std::endl;

 return match;

}
bool AddSSMLVar::SetDLTrigger(TTHbb::Event* event){

  int year = 0;
  int runNumber = (int)(event->m_info->runNumber);
  if (!isData) runNumber = event->intVariable("randomRunNumber");

  bool trig_el=false,trig_mu=false, trig_ee=false, trig_em=false, trig_mm=false;
  bool trig=false;

  if ( runNumber>=270000 &&runNumber <290000){
    year = 15;
    trig_el = event->charVariable("HLT_e24_lhmedium_L1EM20VH")||event->charVariable("HLT_e60_lhmedium")||event->charVariable("HLT_e120_lhloose");
    trig_ee = event->charVariable("HLT_2e12_lhloose_L12EM10VH");
    trig_em = event->charVariable("HLT_e7_lhmedium_mu24")||event->charVariable("HLT_e17_lhloose_mu14");
    trig_mu = event->charVariable("HLT_mu50");
    trig_mm = event->charVariable("HLT_2mu10")||event->charVariable("HLT_mu18_mu8noL1");
  }
  else if ( runNumber>=290000 &&runNumber <= 320000){
    year = 16;
    trig_el = event->charVariable("HLT_e60_lhmedium_nod0")||event->charVariable("HLT_e140_lhloose_nod0");
    trig_ee = event->charVariable("HLT_2e17_lhvloose_nod0");
    trig_em = event->charVariable("HLT_e17_lhloose_nod0_mu14")||event->charVariable("HLT_e7_lhmedium_nod0_mu24");
    trig_mu = event->charVariable("HLT_mu50");
    trig_mm = event->charVariable("HLT_2mu14")||event->charVariable("HLT_mu22_mu8noL1");
    /*
    int eventNumber= event->intVariable("eventNumber");
    if (eventNumber==1217 || (trig_mu || trig_mm)){
      std::cout << "- eventNumber " << eventNumber << std::endl;
      std::cout << "- runNumber : " << event->intVariable("runNumber") << std::endl;
      std::cout << "-- randomNumber : " << runNumber << std::endl;
      std::cout << "- trig_mu : " << trig_mu << ", trig_mm : " << trig_mm << std::endl;
      std::cout << "HLT_mu50 : "<< event->charVariable("HLT_mu50")	<< std::endl; // event->charVariable("HLT_mu50") << std::endl;
      std::cout << "HLT_2mu14 : "<< event->charVariable("HLT_2mu14") << std::endl;
      std::cout << "HLT_mu22_mu8noL1 : "<< event->charVariable("HLT_mu22_mu8noL1") << std::endl;
      if (event->charVariable("HLT_mu50")==1) std::cout << "HLT_mu50 == 1" << std::endl;
      else std::cout << "HLT_mu50 ==0" << std::endl;
    }*/


  }
  else if ( runNumber>=320000 &&runNumber <= 341000){
   year = 17;
   trig_el = event->charVariable("HLT_e60_lhmedium_nod0")||event->charVariable("HLT_e140_lhloose_nod0");
   trig_ee = event->charVariable("HLT_2e24_lhvloose_nod0");
   trig_em = event->charVariable("HLT_e17_lhloose_nod0_mu14")||event->charVariable("HLT_e7_lhmedium_nod0_mu24");
   trig_mu = event->charVariable("HLT_mu50");
   trig_mm = event->charVariable("HLT_2mu14")||event->charVariable("HLT_mu22_mu8noL1");
  }
  else if ( runNumber>=340000 &&runNumber <= 370000){
   year = 18;
   trig_el = event->charVariable("HLT_e60_lhmedium_nod0")||event->charVariable("HLT_e140_lhloose_nod0");
   trig_ee = event->charVariable("HLT_2e24_lhvloose_nod0")||event->charVariable("HLT_2e17_lhvloose_nod0_L12EM15VHI");
   trig_em = event->charVariable("HLT_e17_lhloose_nod0_mu14")||event->charVariable("HLT_e7_lhmedium_nod0_mu24");
   trig_mu = event->charVariable("HLT_mu50");
   trig_mm = event->charVariable("HLT_2mu14")||event->charVariable("HLT_mu22_mu8noL1");
  }

  if (event->intVariable("SSee") ==1      || event->intVariable("loose_SSee") == 1 || event->intVariable("OSee") ==1)     trig = (trig_el || trig_ee);
  else if (event->intVariable("SSem") ==1 || event->intVariable("loose_SSem") == 1 || event->intVariable("OSem") ==1) trig = (trig_el || trig_em || trig_mu);
  else if (event->intVariable("SSmm") ==1 || event->intVariable("loose_SSmm") == 1) trig = (trig_mu || trig_mm);
  else if (event->intVariable("eee") ==1  || event->intVariable("loose_eee") == 1)  trig = (trig_el || trig_ee);
  else if (event->intVariable("eem") ==1  || event->intVariable("loose_eem") == 1)  trig = (trig_el || trig_ee || trig_em || trig_mu);
  else if (event->intVariable("emm") ==1  || event->intVariable("loose_emm") == 1)  trig = (trig_el || trig_mm || trig_em || trig_mu);
  else if (event->intVariable("mmm") ==1  || event->intVariable("loose_mmm") == 1)  trig = (trig_mm || trig_mu);



  event->intVariable("runYear") = year;
  event->intVariable("randomRunNumber") = runNumber;

  return trig;

}

void AddSSMLVar::WeightJetMultiplicity(TTHbb::Event* event){

   float WJM = -999.0;

   if ( WJM_DFct() != 0) WJM = WJM_NFct(event)/WJM_DFct();

   event->floatVariable("WeightJetMultiplicity") = WJM;

}
float AddSSMLVar::WJM_NFct(TTHbb::Event* event){

  TH1F f ("function", "WJM_Fct", 10000, 25*GeV, 125*GeV);

   for (int i_bin = 1; i_bin < f.GetSize()-1; i_bin++) {
     float p_th = f.GetXaxis()->GetBinCenter(i_bin);
     float val = NjetPt(p_th,event)*p_th;
     f.SetBinContent(i_bin, val);
   }

  float intel = f.Integral();
  return intel;

}
float AddSSMLVar::WJM_DFct(){

  TH1F f("function", "WJM_Fct1", 10000, 25*GeV, 125*GeV);

  for (int i_bin = 1; i_bin < f.GetSize()-1; i_bin++) {
    float p_th = f.GetXaxis()->GetBinCenter(i_bin);
    float val = p_th;
    f.SetBinContent(i_bin, val);
  }

  float intel = f.Integral();
  return intel;
}
int AddSSMLVar::NjetPt(float pT_th, TTHbb::Event* event)
{
  std::shared_ptr<Jet> jet;
  unsigned int jets_n = event->m_jets.size();
  int njet = 0;

  for (unsigned int i_jet = 0; i_jet <jets_n; i_jet ++) {
    jet = event->m_jets.at(i_jet);
    if (jet->Pt()>pT_th) njet++;
  }

  return njet;
}


void AddSSMLVar::SetEventLevelLeptonVar(TTHbb::Event* event, std::string type, std::string name,
                                        std::string prefix, unsigned int N) {
  for (unsigned int i = 0; i < N; i++) {
    if (type == "int")
      event->intVariable(prefix + std::to_string(i) + "_" + name) =
          -std::numeric_limits<int>::max();
    else if (type == "float")
      event->floatVariable(prefix + std::to_string(i) + "_" + name) =
          -std::numeric_limits<float>::max();
    else if (type == "char")
      event->charVariable(prefix + std::to_string(i) + "_" + name) = -1;
  }
}

float AddSSMLVar::GetMllPVCO(float m0,float m1){


   float mll=m0*( m0>0 && ( m1<0 || (m1>0 && m1>m0)) )+
             m1*( m1>0 && ( m0<0 || (m0>0 && m1<m0)) )+
             -1*( m0<0 && m1<0);


   return mll;
}

}  // namespace TTHbb
