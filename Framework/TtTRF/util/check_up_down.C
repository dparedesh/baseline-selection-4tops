
#include "AtlasStyle.C"
#include "AtlasLabels.C"
#include "AtlasUtils.C"
#include "./util/TtTRFUtil.C"

void check_up_down(string sample = "mc_mc16a", string pref="5j", string systpref="pu_PRW_DATASF", string systpost="" ){
  SetAtlasStyle();
  void SetPercHist(TH1F* hnom, TH1F*hup, TH1F*hdown,TH1F*href=0);

  string nominal="nominal";
  if (sample.find("mc")!=string::npos) nominal="ttPy8";
  TFile *_file0 = 0x0; 
  _file0 = TFile::Open( ("data/ttTRFmaps2018_"+sample+"_"+pref+"__"+systpref+"_Up"+systpost+".root").c_str());
  if (!_file0) _file0 = TFile::Open( ("data/ttTRFmaps2018_"+sample+"_"+pref+"__"+systpref+systpost+".root").c_str());
  TFile *_file1 = TFile::Open( ("data/ttTRFmaps2018_"+sample+"_"+pref+"__"+nominal+".root").c_str());
  TFile *_file2=0x0;
  _file2 = TFile::Open( ("ttTRFmaps2018_"+sample+"_"+pref+"__"+systpref+"_Down"+systpost+".root").c_str());
  TH1F*hnom=(TH1F*)_file1->Get("h_eff_pt");
  TH1F*hup=(TH1F*)_file0->Get("h_eff_pt");
  hup->SetLineColor(4);
  TH1F*hnom_orig=(TH1F*)hnom->Clone("hnom_orig");
  TH1F*hup_orig=(TH1F*)hup->Clone("hup_orig");
   hnom->Smooth();
   hup->Smooth();

  TH1F*hdown=0x0,*hdown_orig=0x0;
  if (_file2) {
    hdown=(TH1F*)_file2->Get("h_eff_pt");
    hdown->SetName("h_down");
    hdown->SetLineColor(2);
    hdown_orig=(TH1F*)hdown->Clone("hdown_orig");
    hdown->Smooth();
  }
  else {
     hdown = (TH1F*) hnom->Clone("hdown");
     hdown->Scale(2.0);
     hdown->Add(hup,-1);
     hdown->SetLineColor(2);
     // in this case down already smoothed !
     hdown_orig = (TH1F*) hnom_orig->Clone("hdown_orig");
     hdown_orig->Scale(2.0);
     hdown_orig->Add(hup_orig,-1);
     hdown_orig->SetLineColor(2); 
     for (int ii=1; ii<= hdown->GetNbinsX(); ii++ ) {
       hdown->SetBinError(ii,hup->GetBinError(ii));
       hdown_orig->SetBinError(ii,hup_orig->GetBinError(ii));
     }
  }
  SetPercHist(hnom_orig,hup_orig,hdown_orig,hnom);
  SetPercHist(hnom,hup,hdown);
 
  double gmax=GetMaximumManually(hup_orig), gmin=GetMaximumManually(hdown_orig,-1.0), inup=hup_orig->Integral();
  if( 0> inup){
     gmax=GetMaximumManually(hdown_orig), gmin=GetMaximumManually(hup_orig,-1.0);
  }
  hnom->SetMaximum(gmax);
  hnom->SetMinimum(gmin);
  TCanvas*c1 = new TCanvas("c1","c1");
  c1->SetLogx();
  string bmul="2b";
  if (pref.find("4b")!=std::string::npos) bmul="3b";
  hnom->GetYaxis()->SetTitle( ("#Delta #varepsilon_{b}^{#geq" + bmul + "} (%)" ).c_str()  );
  hnom->Draw();
  hup->Draw("hist same");
  hup->SetMaximum(gmax);
  hup->SetMinimum(gmin);
  hdown->SetMaximum(gmax);
  hdown->SetMinimum(gmin);
  hdown->Draw("hist same");

   hnom_orig->SetLineStyle(2);
    hup_orig->SetLineStyle(2);
  hdown_orig->SetLineStyle(2);
  hdown_orig->Draw("hist same");
    hup_orig->Draw("hist same");
   hnom_orig->Draw("hist same");

  double xpos=0.62, ypos=0.87;
  ATLASLabelNew(xpos,ypos, (char*)(" Internal"), 1, gStyle->GetTextSize()*1.5);
  string channel="Single Lepton";
  if (pref.find("dilep")!=std::string::npos) channel="Dilepton";
  myText(xpos,ypos-0.04,1,(channel+", "+systpref+systpost).c_str());
  c1->SaveAs(("plots/Systematics/Syst_"+sample+"_"+pref+"_"+systpref+systpost+".pdf").c_str());
  c1->SaveAs(("plots/Systematics/Syst_"+sample+"_"+pref+"_"+systpref+systpost+".png").c_str());
}

void SetPercHist(TH1F* hnom, TH1F*hup, TH1F*hdown, TH1F*href=0x0){

  TH1F*nomstrip;
  if(href==0x0) nomstrip = (TH1F*)hnom->Clone("nomstrip");
  else nomstrip = (TH1F*)href->Clone("nomstrip");
  for (int ii=1; ii<= nomstrip->GetNbinsX(); ii++ ) nomstrip->SetBinError(ii,0);
  hup->Add(nomstrip,-1);
  hdown->Add(nomstrip,-1);
  hnom->Add(nomstrip,-1);
  hup->Divide(nomstrip);
  hdown->Divide(nomstrip);
  hnom->Divide(nomstrip);
  hup->Scale(100.);
  hdown->Scale(100.);
  hnom->Scale(100.);
}
