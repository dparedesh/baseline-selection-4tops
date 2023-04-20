void check_diff(){
TCanvas *cc1 = new TCanvas("c1","c1",800,900);
TPad * cup =new TPad("up","up",0,0.33,1,1);
TPad * cdown =new TPad("down","down",0,0,1,0.33);
cup->Draw();
cdown->Draw();
cup->cd();
TFile *_file0 = TFile::Open("data/ttTRFmaps2018_mc_mc16a_5j__ttPy8.root");
TFile *_file1 = TFile::Open("datattHntup/ttTRFmaps2018_mc_5j__ttPy8.root");
TH1F* hptold=(TH1F*)_file1->Get("h_eff_Nj_extrap");
TH1F* hptnew=(TH1F*)_file0->Get("h_eff_Nj_extrap");
hptold->Draw();
hptnew->SetMarkerColor(7);
hptnew->SetLineColor(7);
hptnew->Draw("same");
cc1->SetLogx();
cdown->cd();
TH1F*hratio=(TH1F*)hptnew->Clone("hratio");
hratio->Divide(hptold);
double ymin=1, ymax=1,binc,bine;
for (int ibin=1; ibin<=(hratio->GetNbinsX()); ibin++)
{
  binc=hratio->GetBinContent(ibin);
  bine=hratio->GetBinError(ibin);
  if (ymin>binc-bine) ymin=binc-bine;
  if (ymax<binc+bine) ymax=binc+bine;
}
hratio->SetMaximum(ymax+0.05);
hratio->SetMinimum(ymin-0.05);
hratio->Draw();
}

