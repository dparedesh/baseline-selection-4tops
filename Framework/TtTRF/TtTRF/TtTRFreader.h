#ifndef TTTRFREADER_H
#define TTTRFREADER_H

#include <string>
#include "TH1D.h"
#include "TH2F.h"
#include "TMath.h"

class TtTRFreader {
public:
  TtTRFreader(std::string fileName,std::string parametrisation="pt/eta/dRj");
  ~TtTRFreader(){};
  
  float GetEff(float v1,float v2,float v3,float v4, float v5);
  
protected:
  float GetContentAt(TH1D*h1, float v1);
  float GetContentAt(TH2F*h12, float v1, float v2);

private:
  TH1D *h0; // tot
  TH1D *h1;
  TH1D *h2;
  TH1D *h3;
  TH1D *h4;
  TH1D *h5;
  TH1D *h6;
  TH2F *h12;
  TH2F *h12_II;
  TH2F *h13;
  TH2F *h13_II;

  int conversion;

  float V1,V2,V3,V4;

  float btageff0;
}; 

#endif
