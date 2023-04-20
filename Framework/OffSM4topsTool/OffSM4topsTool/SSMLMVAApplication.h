#ifndef _SSMLMVAAPPLICATION_H_
#define _SSMLMVAAPPLICATION_H_

#include "TTHbbObjects/Lepton.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbObjects/Jet.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#include <xgboost/c_api.h>

// To include Xgboost, taken from xgboost/c_api.h, must be identical!
typedef void *BoosterHandle;
typedef void *DMatrixHandle;
typedef uint64_t bst_ulong;

namespace SM4t {
class  SSMLMVAApplication {
 public:
   SSMLMVAApplication(){};
   ~SSMLMVAApplication(){};
   void initialise();
   void ApplyBDT(TTHbb::Event* event);
   void finalise();
 private:

   float Prediction(BoosterHandle *boost, std::vector<float> features);
   std::vector<double> Prediction_multiclass(BoosterHandle *boost, std::vector<float> features);


   // SM BDT

    const char *modelpath_SMBDT_Train_even_Test_odd;
    const char *modelpath_SMBDT_Train_odd_Test_even;
    BoosterHandle *m_xgboost_SMBDT_Train_even_Test_odd =  nullptr;
    BoosterHandle *m_xgboost_SMBDT_Train_odd_Test_even =  nullptr;


     // // BSM BDT
     const char *modelpath_BSMBDT_Train_fold234_Test_fold1;
     const char *modelpath_BSMBDT_Train_fold134_Test_fold2;
     const char *modelpath_BSMBDT_Train_fold124_Test_fold3;
     const char *modelpath_BSMBDT_Train_fold123_Test_fold4;

     BoosterHandle *m_xgboost_BSMBDT_Train_fold234_Test_fold1 =  nullptr;
     BoosterHandle *m_xgboost_BSMBDT_Train_fold134_Test_fold2 =  nullptr;
     BoosterHandle *m_xgboost_BSMBDT_Train_fold124_Test_fold3 =  nullptr;
     BoosterHandle *m_xgboost_BSMBDT_Train_fold123_Test_fold4 =  nullptr;


//including training variables
   float HT_jets_noleadjet;
   float met_met;
   float leading_jet_pT;
   float leading_bjet_pT;
   float lep_0_pt;
   float jet_sum_DL1r_Continuous;
   float deltaR_lb_max;
   float deltaR_ll_min;
   float deltaR_bj_min;
   float deltaR_ll_sum;
   float jet_1_pt;
   float jet_5_pt;
   float SphericityJets;
   float SphericityXYJets;
   float HT_all;
   float jet_sum_DL1r_Continuous_leadingFour;
   float nJets;

   // mH for parametrization
   float mH400 = 400.;
   float mH500 = 500.;
   float mH600 = 600.;
   float mH700 = 700.;
   float mH800 = 800.;
   float mH900 = 900.;
   float mH1000 = 1000.;

   };
}

#endif
