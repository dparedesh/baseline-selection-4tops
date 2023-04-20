void eff_from_cont(){

   TCanvas *c = new TCanvas("c","c",800, 600);

   TFile *_file0 = TFile::Open("ttTRFmaps2018_mc_5j__ttPy8.root");
   TTree * teff = (TTree*) _file0->Get("teff");

   if (!teff) return;

   TH2F*htag=new TH2F("htag","htag",20,3.2189,7.5,20,1.8,25);
   TH2F*htot=new TH2F("htot","htot",20,3.2189,7.5,20,1.8,25);

   teff->Draw("teff_dRjNj:TMath::Log(teff_pt)>>htag","teff_weight*(teff_istagged)","goff");
   teff->Draw("teff_dRjNj:TMath::Log(teff_pt)>>htot","teff_weight","goff");

   Double_t conttot[21],conttag[21];
  
   conttot[0]=htot->GetMinimum();
   conttag[0]=htag->GetMinimum();
   conttot[20]=htot->GetMaximum();
   conttag[20]=htag->GetMaximum();

   for (int icont=2; icont<21; icont++){
        conttot[icont]=(conttot[20]-conttot[0])/19.0 * (icont-1) + conttot[0];
        conttag[icont]=(conttag[20]-conttag[0])/19.0 * (icont-1) + conttag[0];
   }
   double frac=0.25;
   conttot[1]=((1-frac)*conttot[0]+frac*conttot[2]);
   conttag[1]=((1-frac)*conttag[0]+frac*conttag[2]);

   for (int icont=0; icont<21; icont++)
        cout << conttot[icont] << "; " << conttag[icont] << endl;
   
   htag->SetContour(21, conttag);
   htot->SetContour(21, conttot);


   htot->Draw("cont z list");

   c->Update();

   // Get Contours
   TObjArray *conts = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");
   TList* contLevel = NULL;
   TGraph* curv     = NULL;
   TGraph* gc       = NULL;

   Int_t nGraphs    = 0;
   Int_t TotalConts = 0;

   if (conts == NULL){
      printf("*** No Contours Were Extracted!\n");
      TotalConts = 0;
      return 0;
   } else {
      TotalConts = conts->GetSize();
   }

   printf("TotalConts = %d\n", TotalConts);

   for(i = 0; i < TotalConts; i++){
      contLevel = (TList*)conts->At(i);
      printf("Contour %d has %d Graphs\n", i, contLevel->GetSize());
      nGraphs += contLevel->GetSize();
   }

   nGraphs = 0;

   TCanvas* c1 = new TCanvas("c1","Contour List",610,0,600,600);
   c1->SetTopMargin(0.15);
 
   TGraph * hr = new TGraph("hr","hr");
   float xval0=4,yval0=4;
   hr->SetPoint(0, xval0, yval0);
   hr->Draw();

   for(i = 0; i < TotalConts; i++){
      contLevel = (TList*)conts->At(i);
      zval0 = contours[i];
      printf("Z-Level Passed in as:  Z = %f\n", zval0);

      // Get first graph from list on curves on this level
      curv = (TGraph*)contLevel->First();
      for(j = 0; j < contLevel->GetSize(); j++){
         cout << "Distance from graph " << i << " is " << curv->DistancetoPrimitive( xval0, yval0) << endl;
         if (zval0<0) curv->SetLineColor(kRed);
         if (zval0>0) curv->SetLineColor(kBlue);
         nGraphs ++;
         printf("\tGraph: %d  -- %d Elements\n", nGraphs,curv->GetN());

         // Draw clones of the graphs to avoid deletions in case the 1st
         // pad is redrawn.
         gc = (TGraph*)curv->Clone();
         gc->Draw("C");

         sprintf(val,"%g",zval0);
         l.DrawLatex(xval0,yval0,val);
         curv = (TGraph*)contLevel->After(curv); // Get Next graph
      }
   }
   c1->Update();
   printf("\n\n\tExtracted %d Contours and %d Graphs \n", TotalConts, nGraphs );



   htag->Draw("cont3");
   htot->SetLineColor(3);
   htot->Draw("cont3same");
}
/*
teff->Draw("teff_pt>>htag(10,25,700","teff_weight*(teff_istagged)","goff")
htag->Divide(htot)
htag->Draw()
h_eff_pt->Draw("same")
h_eff_pt->SetLineColor(3)
h_eff_pt->SetMarkerColor(3)
.q
teff->Draw("teff_dRjNj:TMath::Log(teff_pt)>>g13(40,3.2189,7.5,40,1.8,25)","teff_weight*(teff_istagged)","colz")
h13->Divide(f13)
h13->Draw("colz")
h13->SetMaximum(0.1)
h13->Draw("colz")
h13->SetLineColor(1)
h13->DrawCopy("cont3same")
.q
TFile *_file0 = TFile::Open("ttTRFmaps2018_mc_5j__ttPy8.root")
teff->Draw("teff_dRjNj:TMath::Log(teff_pt)>>h13(20,3.2189,7.5,20,1.8,25)","teff_weight*(teff_istagged)","cont3")
teff->Draw("teff_dRjNj:TMath::Log(teff_pt)>>f13(20,3.2189,7.5,20,1.8,25)","teff_weight","cont3same")
f13->SetLineColor(3)
f13->SetLineStyle(2)
f13->GetContourLevel(0)
f13->GetContourLevel(1)
f13->GetContourLevel(2)
TH2F*eff2d=(TH2F*)f13->Clone("eff2d")
eff2d->Divide(h13,f13)
eff2d->Draw("colzsame")
eff2d->Draw("colz")
eff2d->SetMaximum(0.15)
eff2d->Draw("colz")
h13->Draw("cont3same")
f13->Draw("cont3same")
f13->GetMinimum()
f13->GetMaximum()
g13->GetMaximum()
h13->GetMaximum()
h13->GetMinimum()
*/
