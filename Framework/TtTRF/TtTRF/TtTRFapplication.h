#ifndef TTTRFAPPLICATION_H
#define TTTRFAPPLICATION_H

#include <map>

#include "CustomTRF/CustomTRF.h"
#include "TtTRF/TtTRFreader.h"

#include "TLorentzVector.h"

class TtTRFapplication {
public:
  TtTRFapplication(int ntags=3,std::string path=".",std::string sample="data",std::string systematic="nominal",
                   std::string selection="5j6j",std::string splitting="split3b4b",std::string parametrisation="pt/eta/dRj",
                   bool debug=false);
  ~TtTRFapplication(){};
  
//   void AddJet(int idx,float pt,float eta,float phi,float e,float weight,int istagged=-1);
  void AddJet(int idx,float pt,float eta,float phi,float e,int istagged=-1);
  void Reset();
  void Evaluate();
  float GetTRFweight();
  bool GetJetIsTagged(int idx);
  int GetNBTags();
  float GetTRFweight(std::string name);
  bool GetJetIsTagged(int idx, std::string name);
  int GetNBTags(std::string name);
  float GetJetVar(int idx, string Var);

  void GetParametrisations(std::vector<std::string> * param);
  
private:
  void SetParametrisations(std::string);
  bool m_debug;
  
  std::map<std::string, TtTRFreader *>m_TRFreader0;
  std::map<std::string, TtTRFreader *>m_TRFreader1;
//   std::vector<TtTRFreader*> m_systTRFreader0;
//   std::vector<TtTRFreader*> m_systTRFreader1;

  std::map<std::string, CustomTRF *> m_TRF;
  std::map<std::string, CustomTRF *> m_TRF0;
  std::map<std::string, CustomTRF *> m_TRF1;
  std::map<std::string, CustomTRF *> m_TRF2;
  
  std::vector<int> m_jet_index;
  std::vector<TLorentzVector> m_jet;
  std::vector<float> m_jet_weight;
  std::vector<int> m_jet_istagged;
  std::vector<float> m_jet_dRjmin;
  
  std::map<std::string, double> m_weight;
  int m_ntags;
  int m_incl;
  std::string sel;
  std::vector<std::string> m_parametrisations;
  float m_dRjmin[2];
   
  std::map<std::string, std::vector<int> > m_jet_istagged_ttTRF;
  std::map<std::string, int> m_nBtags_ttTRF;
  
  bool m_split3b;
  bool m_split4b;
  bool m_incltoexcl;
}; 

#endif
