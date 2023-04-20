#include "OffSM4topsTool/SSMLMVAApplication.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"


// to check the os system, to decide which file to model use
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string>

// include xgboost
#include <xgboost/c_api.h>

#define safe_xgboost(call) {                                            \
int err = (call);                                                       \
if (err != 0) {                                                         \
  fprintf(stderr, "%s:%d: error in %s: %s\n", __FILE__, __LINE__, #call, XGBGetLastError()); \
  exit(1);                                                              \
}                                                                       \
}

auto runSMBDT = 1;
auto runBSMBDT = 1;
auto verbose = 0;

namespace SM4t {


void SSMLMVAApplication::initialise()
{

    // Variable to check the environment of the system
    struct utsname sysinfo;
    if(uname(&sysinfo)) exit(9);
    std::string sysnode(sysinfo.nodename);
    std::string lxpluse_node("lxplus");
    std::string mich_node("umt");
    std::string mich_condor("aglt2");

    if (runSMBDT){

        modelpath_SMBDT_Train_even_Test_odd = "../../MVAmodels_SSML/SMBDT/SMBDT_Official_AT212120_NT800_Depth3_SubSam0.6_colTree0.4_eta0.03_train_even_test_odd.h5";
        modelpath_SMBDT_Train_odd_Test_even = "../../MVAmodels_SSML/SMBDT/SMBDT_Official_AT212120_NT800_Depth3_SubSam0.6_colTree0.4_eta0.03_train_odd_test_even.h5";

    }
    if (runBSMBDT){

        modelpath_BSMBDT_Train_fold234_Test_fold1 = "../../MVAmodels_SSML/BSMBDT/BSMBDT_Official_AT212120_otherBKG07_NT1000_Depth4_SubSam0.6_eta0.01_train_fold234_test_fold1.h5";
        modelpath_BSMBDT_Train_fold134_Test_fold2 = "../../MVAmodels_SSML/BSMBDT/BSMBDT_Official_AT212120_otherBKG07_NT1000_Depth4_SubSam0.6_eta0.01_train_fold134_test_fold2.h5";
        modelpath_BSMBDT_Train_fold124_Test_fold3 = "../../MVAmodels_SSML/BSMBDT/BSMBDT_Official_AT212120_otherBKG07_NT1000_Depth4_SubSam0.6_eta0.01_train_fold124_test_fold3.h5";
        modelpath_BSMBDT_Train_fold123_Test_fold4 = "../../MVAmodels_SSML/BSMBDT/BSMBDT_Official_AT212120_otherBKG07_NT1000_Depth4_SubSam0.6_eta0.01_train_fold123_test_fold4.h5";

    }



    // Initialize the booster
    if (runSMBDT){

          m_xgboost_SMBDT_Train_even_Test_odd = new BoosterHandle();
          safe_xgboost(XGBoosterCreate(NULL, 0, * &m_xgboost_SMBDT_Train_even_Test_odd));
          safe_xgboost(XGBoosterLoadModel(*m_xgboost_SMBDT_Train_even_Test_odd, modelpath_SMBDT_Train_even_Test_odd));

          m_xgboost_SMBDT_Train_odd_Test_even = new BoosterHandle();
          safe_xgboost(XGBoosterCreate(NULL, 0, * &m_xgboost_SMBDT_Train_odd_Test_even));
          safe_xgboost(XGBoosterLoadModel(*m_xgboost_SMBDT_Train_odd_Test_even, modelpath_SMBDT_Train_odd_Test_even));

    }
    if (runBSMBDT){

      m_xgboost_BSMBDT_Train_fold234_Test_fold1 = new BoosterHandle();
      safe_xgboost(XGBoosterCreate(NULL, 0, * &m_xgboost_BSMBDT_Train_fold234_Test_fold1));
      safe_xgboost(XGBoosterLoadModel(*m_xgboost_BSMBDT_Train_fold234_Test_fold1, modelpath_BSMBDT_Train_fold234_Test_fold1));

      m_xgboost_BSMBDT_Train_fold134_Test_fold2 = new BoosterHandle();
      safe_xgboost(XGBoosterCreate(NULL, 0, * &m_xgboost_BSMBDT_Train_fold134_Test_fold2));
      safe_xgboost(XGBoosterLoadModel(*m_xgboost_BSMBDT_Train_fold134_Test_fold2, modelpath_BSMBDT_Train_fold134_Test_fold2));

      m_xgboost_BSMBDT_Train_fold124_Test_fold3 = new BoosterHandle();
      safe_xgboost(XGBoosterCreate(NULL, 0, * &m_xgboost_BSMBDT_Train_fold124_Test_fold3));
      safe_xgboost(XGBoosterLoadModel(*m_xgboost_BSMBDT_Train_fold124_Test_fold3, modelpath_BSMBDT_Train_fold124_Test_fold3));

      m_xgboost_BSMBDT_Train_fold123_Test_fold4 = new BoosterHandle();
      safe_xgboost(XGBoosterCreate(NULL, 0, * &m_xgboost_BSMBDT_Train_fold123_Test_fold4));
      safe_xgboost(XGBoosterLoadModel(*m_xgboost_BSMBDT_Train_fold123_Test_fold4, modelpath_BSMBDT_Train_fold123_Test_fold4));

    }

}

float SSMLMVAApplication::Prediction(BoosterHandle *boost, std::vector<float> features){

    DMatrixHandle dmat;
    safe_xgboost(XGDMatrixCreateFromMat(&features[0], 1, features.size(), -999.0, &dmat));
    bst_ulong out_len = 0;
    const float* out_result = NULL;
    safe_xgboost(XGBoosterPredict(*boost, dmat, 0, 0, 0 ,&out_len, &out_result));
    safe_xgboost(XGDMatrixFree(dmat));

    float weight = out_result[0];

    return weight;
}

std::vector<double> SSMLMVAApplication::Prediction_multiclass(BoosterHandle *boost, std::vector<float> features){

    DMatrixHandle dmat;
    safe_xgboost(XGDMatrixCreateFromMat(&features[0], 1, features.size(), -999.0, &dmat));
    bst_ulong out_len = 0;
    const float* out_result = NULL;
    safe_xgboost(XGBoosterPredict(*boost, dmat, 0, 0, 0 ,&out_len, &out_result));
    std::vector<double> outputVector(out_result, out_result + out_len);
    safe_xgboost(XGDMatrixFree(dmat));

    return outputVector;
}

void SSMLMVAApplication::ApplyBDT(TTHbb::Event* event)
{


    HT_jets_noleadjet= event->floatVariable("HT_jets_noleadjet");
    met_met = event->met_met;
    leading_jet_pT = event->floatVariable("leading_jet_pT");
    leading_bjet_pT =event->floatVariable("leading_bjet_pT");
    lep_0_pt = event->floatVariable("lep_0_pt");
    deltaR_lb_max = event->floatVariable("deltaR_lb_max");
    deltaR_ll_min = event->floatVariable("deltaR_ll_min");
    deltaR_bj_min = event->floatVariable("deltaR_bj_min");
    deltaR_ll_sum = event->floatVariable("deltaR_ll_sum");
    jet_5_pt = event->floatVariable("jet_5_pt");
    jet_1_pt = event->floatVariable("jet_1_pt");
    jet_sum_DL1r_Continuous = (float) event->intVariable("jet_sum_DL1r_Continuous");
    SphericityJets = event->floatVariable("SphericityJets");
    SphericityXYJets = event->floatVariable("SphericityXYJets");
    HT_all = event->floatVariable("HT_all");
    nJets = event->intVariable("nJets");
    jet_sum_DL1r_Continuous_leadingFour = event->floatVariable("jet_sum_DL1r_Continuous_leadingFour");
    // Get eventnumber in a better way
    auto eventNumber = event->m_info->eventNumber;

    // This is buggy - don't use int casting!!!!!!!!!!!!!!!!!!!
    /////////////////// auto eventNumber = (int)(event->m_info->eventNumber);
    //

    // Define input variable vector for SM BDT
    std::vector<float> input_values_SMBDT = {jet_sum_DL1r_Continuous_leadingFour,nJets,deltaR_ll_min,leading_jet_pT,leading_bjet_pT,lep_0_pt,met_met,deltaR_ll_sum,HT_jets_noleadjet,deltaR_lb_max,jet_5_pt,deltaR_bj_min,jet_1_pt};
    // std::vector<float> input_values_SMBDT_nominal = {jet_sum_DL1r_Continuous,deltaR_ll_min,leading_jet_pT,leading_bjet_pT,lep_0_pt,met_met,deltaR_ll_sum,HT_jets_noleadjet,deltaR_lb_max,jet_5_pt,deltaR_bj_min,jet_1_pt};

    // Define BDTs
    float SMBDT(-999);

    // float SMBDT_deco_originalW(-999);
    // float SMBDT_nominal(-999);

    if (runSMBDT){

      if (labs(eventNumber)%2==0){
        SMBDT = Prediction(m_xgboost_SMBDT_Train_odd_Test_even, input_values_SMBDT);
      }
      else if (labs(eventNumber)%2!=0){
        SMBDT = Prediction(m_xgboost_SMBDT_Train_even_Test_odd, input_values_SMBDT);
      }
      else{
        std::cout << "Buggy"   << std::endl;
        std::cout << "eventNumber = " << eventNumber  << std::endl;
        std::cout << "eventNumber%2 = " << eventNumber%2  << std::endl;
      }

      event->floatVariable("SMBDT") = SMBDT;


      if(verbose){
            std::cout << "SMBDT = " << SMBDT << std::endl;
      }

    }

    if (runBSMBDT){
      float BSMBDT_400(-999);
      float BSMBDT_500(-999);
      float BSMBDT_600(-999);
      float BSMBDT_700(-999);
      float BSMBDT_800(-999);
      float BSMBDT_900(-999);
      float BSMBDT_1000(-999);

      std::vector<float> input_values_BSMBDT_400  = {deltaR_ll_min,deltaR_ll_sum,SphericityJets,SphericityXYJets,met_met,HT_all,SMBDT,mH400};
      std::vector<float> input_values_BSMBDT_500  = {deltaR_ll_min,deltaR_ll_sum,SphericityJets,SphericityXYJets,met_met,HT_all,SMBDT,mH500};
      std::vector<float> input_values_BSMBDT_600  = {deltaR_ll_min,deltaR_ll_sum,SphericityJets,SphericityXYJets,met_met,HT_all,SMBDT,mH600};
      std::vector<float> input_values_BSMBDT_700  = {deltaR_ll_min,deltaR_ll_sum,SphericityJets,SphericityXYJets,met_met,HT_all,SMBDT,mH700};
      std::vector<float> input_values_BSMBDT_800  = {deltaR_ll_min,deltaR_ll_sum,SphericityJets,SphericityXYJets,met_met,HT_all,SMBDT,mH800};
      std::vector<float> input_values_BSMBDT_900  = {deltaR_ll_min,deltaR_ll_sum,SphericityJets,SphericityXYJets,met_met,HT_all,SMBDT,mH900};
      std::vector<float> input_values_BSMBDT_1000 = {deltaR_ll_min,deltaR_ll_sum,SphericityJets,SphericityXYJets,met_met,HT_all,SMBDT,mH1000};

      if (labs(eventNumber)%4==0){
        BSMBDT_400  = Prediction(m_xgboost_BSMBDT_Train_fold234_Test_fold1, input_values_BSMBDT_400);
        BSMBDT_500  = Prediction(m_xgboost_BSMBDT_Train_fold234_Test_fold1, input_values_BSMBDT_500);
        BSMBDT_600  = Prediction(m_xgboost_BSMBDT_Train_fold234_Test_fold1, input_values_BSMBDT_600);
        BSMBDT_700  = Prediction(m_xgboost_BSMBDT_Train_fold234_Test_fold1, input_values_BSMBDT_700);
        BSMBDT_800  = Prediction(m_xgboost_BSMBDT_Train_fold234_Test_fold1, input_values_BSMBDT_800);
        BSMBDT_900  = Prediction(m_xgboost_BSMBDT_Train_fold234_Test_fold1, input_values_BSMBDT_900);
        BSMBDT_1000 = Prediction(m_xgboost_BSMBDT_Train_fold234_Test_fold1, input_values_BSMBDT_1000);

      }
      else if (labs(eventNumber)%4==1){
        BSMBDT_400  = Prediction(m_xgboost_BSMBDT_Train_fold134_Test_fold2, input_values_BSMBDT_400);
        BSMBDT_500  = Prediction(m_xgboost_BSMBDT_Train_fold134_Test_fold2, input_values_BSMBDT_500);
        BSMBDT_600  = Prediction(m_xgboost_BSMBDT_Train_fold134_Test_fold2, input_values_BSMBDT_600);
        BSMBDT_700  = Prediction(m_xgboost_BSMBDT_Train_fold134_Test_fold2, input_values_BSMBDT_700);
        BSMBDT_800  = Prediction(m_xgboost_BSMBDT_Train_fold134_Test_fold2, input_values_BSMBDT_800);
        BSMBDT_900  = Prediction(m_xgboost_BSMBDT_Train_fold134_Test_fold2, input_values_BSMBDT_900);
        BSMBDT_1000 = Prediction(m_xgboost_BSMBDT_Train_fold134_Test_fold2, input_values_BSMBDT_1000);

      }
      else if (labs(eventNumber)%4==2){
        BSMBDT_400  = Prediction(m_xgboost_BSMBDT_Train_fold124_Test_fold3, input_values_BSMBDT_400);
        BSMBDT_500  = Prediction(m_xgboost_BSMBDT_Train_fold124_Test_fold3, input_values_BSMBDT_500);
        BSMBDT_600  = Prediction(m_xgboost_BSMBDT_Train_fold124_Test_fold3, input_values_BSMBDT_600);
        BSMBDT_700  = Prediction(m_xgboost_BSMBDT_Train_fold124_Test_fold3, input_values_BSMBDT_700);
        BSMBDT_800  = Prediction(m_xgboost_BSMBDT_Train_fold124_Test_fold3, input_values_BSMBDT_800);
        BSMBDT_900  = Prediction(m_xgboost_BSMBDT_Train_fold124_Test_fold3, input_values_BSMBDT_900);
        BSMBDT_1000 = Prediction(m_xgboost_BSMBDT_Train_fold124_Test_fold3, input_values_BSMBDT_1000);

      }

      else if (labs(eventNumber)%4==3){
        BSMBDT_400  = Prediction(m_xgboost_BSMBDT_Train_fold123_Test_fold4, input_values_BSMBDT_400);
        BSMBDT_500  = Prediction(m_xgboost_BSMBDT_Train_fold123_Test_fold4, input_values_BSMBDT_500);
        BSMBDT_600  = Prediction(m_xgboost_BSMBDT_Train_fold123_Test_fold4, input_values_BSMBDT_600);
        BSMBDT_700  = Prediction(m_xgboost_BSMBDT_Train_fold123_Test_fold4, input_values_BSMBDT_700);
        BSMBDT_800  = Prediction(m_xgboost_BSMBDT_Train_fold123_Test_fold4, input_values_BSMBDT_800);
        BSMBDT_900  = Prediction(m_xgboost_BSMBDT_Train_fold123_Test_fold4, input_values_BSMBDT_900);
        BSMBDT_1000 = Prediction(m_xgboost_BSMBDT_Train_fold123_Test_fold4, input_values_BSMBDT_1000);

      }
      else{
        std::cout << "Buggy"   << std::endl;
        std::cout << "eventNumber = " << eventNumber  << std::endl;
        std::cout << "eventNumber%4 = " << eventNumber%4  << std::endl;
      }

      event->floatVariable("BSMBDT_400")  = BSMBDT_400;
      event->floatVariable("BSMBDT_500")  = BSMBDT_500;
      event->floatVariable("BSMBDT_600")  = BSMBDT_600;
      event->floatVariable("BSMBDT_700")  = BSMBDT_700;
      event->floatVariable("BSMBDT_800")  = BSMBDT_800;
      event->floatVariable("BSMBDT_900")  = BSMBDT_900;
      event->floatVariable("BSMBDT_1000") = BSMBDT_1000;


      if (verbose){
          std::cout << "BSMBDT_400 = " << BSMBDT_400
                    << ", BSMBDT_500 = " << BSMBDT_500
                    << ", BSMBDT_600 = " << BSMBDT_600
                    << ", BSMBDT_700 = " << BSMBDT_700
                    << ", BSMBDT_800 = " << BSMBDT_800
                    << ", BSMBDT_900 = " << BSMBDT_900
                    << ", BSMBDT_1000 = " << BSMBDT_1000 << std::endl;
      }


    }


    if (verbose) {
       std::cout << "HT_jets_noleadjet: "<< HT_jets_noleadjet
        <<", met_met: " <<  met_met
        <<", leading_jet_pT: " <<  leading_jet_pT
        <<", leading_bjet_pT: " <<  leading_bjet_pT
        <<", lep_0_pt: " <<  lep_0_pt
        <<", deltaR_lb_max: " <<  deltaR_lb_max
        <<", deltaR_ll_min: " <<  deltaR_ll_min
        <<", deltaR_bj_min: " <<  deltaR_bj_min
        <<", deltaR_ll_sum: " <<  deltaR_ll_sum
        <<", jet_sum_DL1r_Continuous: " <<  jet_sum_DL1r_Continuous
        <<", jet_1_pt: " <<  jet_1_pt
        <<", jet_5_pt: " <<  jet_5_pt
        <<", SphericityJets: " <<  SphericityJets
        <<", SphericityXYJets: " <<  SphericityXYJets
        <<", HT_all: " <<  HT_all
        <<", nJets: " <<  nJets
        <<", jet_sum_DL1r_Continuous_leadingFour: " <<  jet_sum_DL1r_Continuous_leadingFour << std::endl;

    }
}

void SSMLMVAApplication::finalise()
{
  if (runSMBDT){
      XGBoosterFree(*m_xgboost_SMBDT_Train_even_Test_odd);
      XGBoosterFree(*m_xgboost_SMBDT_Train_odd_Test_even);

  }

  if (runBSMBDT){
      XGBoosterFree(*m_xgboost_BSMBDT_Train_fold234_Test_fold1);
      XGBoosterFree(*m_xgboost_BSMBDT_Train_fold134_Test_fold2);
      XGBoosterFree(*m_xgboost_BSMBDT_Train_fold124_Test_fold3);
      XGBoosterFree(*m_xgboost_BSMBDT_Train_fold123_Test_fold4);

  }

}


}
