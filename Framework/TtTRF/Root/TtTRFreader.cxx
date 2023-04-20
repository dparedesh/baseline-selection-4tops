#include "TtTRF/TtTRFreader.h"

#include <iostream>

#include "TDirectory.h"
#include "TFile.h"

TtTRFreader::TtTRFreader(std::string fileName,std::string parametrisation){
  //TDirectory *d0 = gDirectory;
  TFile *f = new TFile(fileName.c_str());
  if (f->IsZombie()) {
      std::cout << "ERROR in TtTRF::TtTRFreader: the specified map file does not exist, exiting ..." << std::endl;
      exit(24);
  }
  conversion=0;
  if (fileName.find("3b")!=std::string::npos) {
    TH1D*h_eff_Nj=(TH1D*)f->Get("h_eff_Nj");
    for (int jbin=h_eff_Nj->GetNbinsX(); jbin>=1; jbin--) if (h_eff_Nj->GetBinContent(jbin)>0.001) conversion=jbin;
    std::cout << "INFO in TtTRF::TtTRFreader: Assuming e2b3b has been measured in nj = "<< conversion << std::endl;
  }
  h0 = 0x0; h1 = 0x0; h2 = 0x0; h3 = 0x0; h4 =0x0; h5 = 0x0; h6 = 0x0;
  h12=0x0; h12_II=0x0; h13=0x0; h13_II=0x0; btageff0 = 0;;
  std::string parsfordist="";
  if(f!=0x0){
    h0 = (TH1D*)f->Get("h_eff");
    if (h0!=0x0)   btageff0 = h0->GetBinContent(1);
    if(parametrisation.find("pt") !=std::string::npos)  { h1 = (TH1D*)f->Get("h_eff_pt"); parsfordist+="pt"; }
    if(parametrisation.find("eta")!=std::string::npos)    h2 = (TH1D*)f->Get("h_eff_eta");
    if(parametrisation.find("dRj")!=std::string::npos)  { h3 = (TH1D*)f->Get("h_eff_dRj"); parsfordist+="dRj";}
    if(parametrisation.find("Nj")!=std::string::npos)     h4 = (TH1D*)f->Get("h_eff_Nj_extrap");
    if(parametrisation.find("dRtj")!=std::string::npos) { h6 = (TH1D*)f->Get("h_eff_dRj_bulk"); parsfordist+="dRtj";}
    if(parametrisation.find("dist")!=std::string::npos) {
                                                          if (parsfordist=="ptdRj") h5 = (TH1D*)f->Get("h_eff_Nj_dist");
                                                          else h5 = (TH1D*)f->Get( ("h_eff_Nj"+parsfordist+"_dist").c_str());
                                                        }
    if(parametrisation.find("2D")!=std::string::npos) {
          if(parametrisation.find("pt") !=std::string::npos) {
                if(parametrisation.find("eta")!=std::string::npos) {
                        if (parametrisation.find("II")!=std::string::npos)
                          h12_II = (TH2F*)f->Get("h_eff_pt_eta_II");
                        else h12 = (TH2F*)f->Get("h_eff_pt_eta");
                        if (h12!=0x0 || h12_II!=0x0){
                          delete h1;
                          h1 = 0x0;
			  delete h2;
                	  h2 = 0x0;
                        }
                        else std::cout << "ERROR in TtTRF::TtTRFreader: 2D parametrisation pt/eta not present."<< std::endl;
                }
                else if(parametrisation.find("dRj")!=std::string::npos){
                        if (parametrisation.find("II")!=std::string::npos) {
                          h13_II = (TH2F*)f->Get("h_eff_pt_dRj_II");
                          if(parametrisation.find("dist")!=std::string::npos) h5 = (TH1D*)f->Get("h_eff_Nj_dist_2DptdRj");
                        }
                        else h13 = (TH2F*)f->Get("h_eff_pt_dRj");
                        if (h13!=0x0 || h13_II!=0x0){
                          delete h1;
                          h1 = 0x0;
			  delete h3;
                	  h3 = 0x0;
                        }
                        else std::cout << "ERROR in TtTRF::TtTRFreader: 2D parametrisation pt/dRj not present."<< std::endl;
                }
          }
          else {
                std::cout << "WARNING in TtTRF::TtTRFreader: No 2D parametrization without pt implemented, leaving 1Dx1D." << std::endl;
          }
    }
  }
  else{
    std::cout << "ttTRF::TtTRFreader: WARNING: File " << fileName << " cannot be openened... Skipping it." << std::endl; 
  }

  if (h1!=0x0) std::cout << "INFO in TtTRF::TtTRFreader: using 1D pt parametrization." << std::endl;
  if (h2!=0x0) std::cout << "INFO in TtTRF::TtTRFreader: using 1D eta parametrization." << std::endl;
  if (h3!=0x0) std::cout << "INFO in TtTRF::TtTRFreader: using 1D dRj parametrization." << std::endl;
  if (h4!=0x0) std::cout << "INFO in TtTRF::TtTRFreader: reweighting using Nj extrapolation." << std::endl;
  if (h5!=0x0) std::cout << "INFO in TtTRF::TtTRFreader: global efficiency takes into account "<< parsfordist << " deformation." << std::endl;
  if (h6!=0x0) std::cout << "INFO in TtTRF::TtTRFreader: using 1D dRtj parametrization." << std::endl;
  if (h12!=0x0 || h12_II!=0x0 ) std::cout << "INFO in TtTRF::TtTRFreader: using 2D eta/pt parametrization." << std::endl;
  if (h13!=0x0 || h13_II!=0x0 ) std::cout << "INFO in TtTRF::TtTRFreader: using 2D eta/dRj parametrization." << std::endl;
//     d0->cd();
//     delete f;
}

//TtTRFreader::~TtTRFreader(){}
  
float TtTRFreader::GetEff(float v1,float v2,float v3, float v4, float v5){
//   if(h0==0x0 || h1==0x0 || h2==0x0 || h3==0x0) return 0;
  if(h0==0x0) return 0;
//   float btageff  = h1->GetBinContent(h1->FindBin(v1));
  float btageff = btageff0;
  if(h1!=0x0) btageff *= GetContentAt(h1, v1) / btageff0;
  if(h2!=0x0) btageff *= GetContentAt(h2, v2) / btageff0;
  if(h3!=0x0) btageff *= GetContentAt(h3, v3) / btageff0;
  if(h6!=0x0) btageff *= GetContentAt(h6, v5) / btageff0;
  if(h12!=0x0) btageff = GetContentAt(h12, TMath::Log(v1), TMath::Abs(v2));
  if(h12_II!=0x0) {
        V1=log(v1/25.0)/4.27666611;
        btageff = GetContentAt(h12_II, V1, TMath::Abs(v2));
  }
  if(h13!=0x0) {
        V1=TMath::Log(v1/25.0)/TMath::Log(1800/25.0);
        V3=(v3-1.8)/(25-1.8);
        btageff = GetContentAt(h13, TMath::ATan(V3/V1), sqrt(V1*V1+V3*V3) );
  }
  if(h13_II!=0x0) {
        V1=log(v1/25.0)/4.27666611;
        V3=(log(v3)-1.9303648555)/1.2885109693;
        btageff = GetContentAt(h13_II, V1, V3);
  }
  if(h4!=0x0) btageff *= GetContentAt(h4, v4+0.5) / btageff0;
  if (h5!=0x0) btageff *= btageff0 / GetContentAt(h5, v4+0.5);

  return btageff;
}

float TtTRFreader::GetContentAt(TH1D*h1, float v1){
  //  return h1->GetBinContent(h1->FindBin(v1));
  double btageff = h1->Interpolate(v1);
  return btageff / (1.0 -conversion * btageff);
}

float TtTRFreader::GetContentAt(TH2F*h12, float v1, float v2){
  double btageff = h12->GetBinContent(h12->FindBin(v1,v2) );
  return btageff / (1.0 -conversion * btageff);
}
