#ifndef _ADDSSMLVAR_H_
#define _ADDSSMLVAR_H_

#include <string>
#include <vector>
#include "OffSM4topsTool/TruthHelpers.h"
#include "OffSM4topsTool/SSMLMVAApplication.h"
#include "OffSM4topsTool/json.hpp"
#include "TTHbbObjects/Event.h"
#include "TTHbbToolManager/ToolBase.h"
#include <TH2F.h>
#include <TH3F.h>
#include <TH2D.h>
#include <TH3D.h>

namespace TTHbb {

class AddSSMLVar : public ToolBase {
  /**
   * @brief Tool for postproduction of SSML mini-ntuples
   * Compute additionnal global variables (e.g. dRlj etc...
   */

 public:
  /// Constructor
  AddSSMLVar(){};

  /// Destructor
  ~AddSSMLVar(){};

  /// Function to initialise objects
  void initialise();

  /// Fill new variables
  void apply(TTHbb::Event* event);

  /// Function to finalise
  void finalise();

 private:
  /// Private parameter
  SM4t::TruthHelpers* m_truthHelper = nullptr;
  bool isData;

  // Add truth mass and sig/bkg labels
  void ClassifySigBkg(TTHbb::Event* event);

  /// Add the deltaR between lepton and closest jet
  void AddLeptonVariables(TTHbb::Event* event);
  void AddJetVariables(TTHbb::Event* event);
  void WeightJetMultiplicity(TTHbb::Event* event);
  void Sphericity(TTHbb::Event* event);

  int NjetPt(float pT_th, TTHbb::Event* event);
  float WJM_NFct(TTHbb::Event* event);
  float WJM_DFct();
  void CalculateMlb(std::vector<TLorentzVector> leptons, std::vector<TLorentzVector> bjets,
                    float& m_lb_0, float& m_lb_1, float& m_lb_2);
  void SetEventLevelLeptonVar(TTHbb::Event* event, std::string type, std::string name,
                              std::string prefix = "lep_", unsigned int N = 3);


  bool SetDLTrigger(TTHbb::Event* event);
  bool SetLTriggerMatching(std::shared_ptr<TTHbb::Lepton> lep,int year);

  inline float GetMTW(std::shared_ptr<TTHbb::Lepton> lep,float met_met,float met_phi){ return sqrt(2 * lep->Pt() * met_met * (1 - cos(lep->Phi() - met_phi)));};
  inline float GetMll(TLorentzVector lep1,TLorentzVector lep2){return (lep1+lep2).M(); };
  inline float GetMllClosestToZ(float m1, float m2){ return (fabs(m1 - 91200) < fabs(m2 - 91200) ? m1 : m2); };
  float GetMllPVCO(float m0,float m1);

  double GetQmisIDWeight(double q1,double q2);
  double GetQmisIDLeptonRate(float eta,float pt,TH2D *pH);
  double GetQmisIDLeptonRate(float eta,float pt,float mll,TH3D *pH);

  float GeV = 1000.;

  bool doChargeFlip;
  TH3D *m_qmisid_COCR_2eta;
  TH2D *m_qmisid_def;
  TH2D *m_qmisid_ttWCR;

  TH3D *m_qmisid_COCR_2eta_syst;
  TH2D *m_qmisid_def_syst;
  TH2D *m_qmisid_ttWCR_syst;

  SM4t::SSMLMVAApplication* m_MVAApplicator = nullptr;

};
}  // namespace TTHbb

#endif
