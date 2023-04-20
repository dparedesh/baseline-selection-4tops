void show_extrap(string filename){

   TFile *_file0 = TFile::Open(("data/"+filename).c_str(),"READ");
   TH1D*h_eff_Nj=(TH1D*)_file0->Get("h_eff_Nj");
   TH1D*h_eff_Nj_extrap=(TH1D*)_file0->Get("h_eff_Nj_extrap");
   h_eff_Nj_extrap->Draw();
   h_eff_Nj->Draw("same");
   h_eff_Nj_extrap->SetDirectory(gROOT);
   h_eff_Nj->SetDirectory(gROOT);
   _file0->Close();
   delete _file0;
}
