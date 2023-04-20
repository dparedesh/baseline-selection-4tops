void save_extrap(string filename="ttTRFmaps2018_mc_mc16a_5j__ttPy8.root"){

   string inclfilename=filename; 
   size_t p5j=filename.find("_5j_"), p4j=filename.find("_4j_");
   if (p5j != string::npos && p4j == string::npos) inclfilename.replace(filename.find("5j")-1,4,"_incl_");
   else if (p5j == string::npos && p4j != string::npos) inclfilename.replace(filename.find("4j")-1,4,"_incl_");
   else {
    cout << "Efficiency map not recognized, exiting ..." << endl;
    return;
   }
   cout << "Opening "<< inclfilename<< " as input ..." << endl;
   TFile *_file0 = new TFile(("data/"+inclfilename).c_str(),"READ");
   cout << "Opening "<< filename<< " as output ..." << endl;
   TFile *_file1 = new TFile(("data/"+filename).c_str(),"UPDATE");

   // look for extrapolation/ distortion histograms
   TIter nextkey( _file0->GetListOfKeys() );
   TKey *key, *oldkey=0;
   while ( (key = (TKey*)nextkey())) {
        //keep only the highest cycle number for each key
        if (oldkey) if (!strcmp(oldkey->GetName(),key->GetName())) continue;
        
        // ok, this key is the only one/highest cycle number
        // read object from this source file
        string keyname = key->GetName();
        TObject *obj = key->ReadObj();
        if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
            // descendant of TH1 -> an efficiency
            TH1D *h1 = (TH1D*)obj;
            // save it if relevant
            if (keyname.find("h_eff_Nj")==0){
               if ( ( keyname.find("dist")   != std::string::npos ) ||
                    ( keyname.find("extrap") == (keyname.size()-6) ) ){
                          h1->SetName(keyname.c_str());
                          h1->SetDirectory(_file1);
                          h1->Write(keyname.c_str(),TObject::kOverwrite);
                         cout << "  >> Histogram "<< keyname << " written to output file."<< endl;
               }
               else  cout << "  >> Histogram "<< keyname << " is an eff. vs Nj, but skipped."<< endl;
            }
        }
        else if ( obj->IsA()->InheritsFrom( TNamed::Class() ) ) {
            // not descendant of TH1 but still TNamed
            TNamed *v1 = (TNamed*)obj;
            // save it if relevant
            if (keyname.find("version")==0){
                v1->SetName(keyname.c_str());
                // v1->SetDirectory(_file1);
                v1->Write(keyname.c_str(),TObject::kOverwrite);
                cout << "  >> Version key "<< keyname << " with title " << v1->GetTitle() << " written to output file."<< endl;
            }
            else  cout << "  >> TNamed "<< keyname << " skipped."<< endl;
        }
   }
   _file1->Close();
   delete _file1;
   delete _file0;
}
