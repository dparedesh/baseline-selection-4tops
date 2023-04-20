  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////=
  //
  // Numerator: weighted number of btagged jets. Remember we are using the ttTRF samples, where nBTags is set to the -predicted-  nbtags ...
  //                 i.e. asking "nbtags>2" should (must) be irrelevant
  //
  //          1)  Definition:                                     1 * f3b + 2 * f4b + ... (nj-2) * f(nj)b
  //                                          e_3b        =   ---------------------------------------------- ,   where [  ] = 1 (or total weight, if "f" are not normalised)
  //                                                            (nj-2) * [ f2b + f3b + f4b + ... f(nj)b ]
  //
  //
  //          2)  Ideal world (and assumed by usual TRF mathematics), also:
  //                                          
  //                                           e_3b                    Prob ( 3b )                 f3b
  //                                        ----------     =      -----------------------  =  ---------------
  //                                         1 -  e_3b            (nj - 2 ) Prob ( 2b )        (nj - 2) f2b
  // 
  //      Similar formulae hold for e_ge4b. If e_3b were e_ge4b, 1) and 2) should give the exact same number, but this is not true for effective efficiencies. 
  //      For e_3b 2) is more appropriate, while (the analogue of) 1) is more appropriate for e_ge4b. 
  //
  //      From an algorithmic point of view, 2) is exactly 1), but stopping both sums to the first term. Also, technically speaking, f3b is retrieved from 3b samples if 
  //      weighted for the corresponding "weight_ttTRF", while f2b from the same sample without the ttTRF weight applied. All other fractions must be retrieved by ge4b samples.
  //
  //      The function retrieve e_3b / e_ge4b (excl = 0 / 1) and, for e_3b, 1) or 2) if either method = 0 or 1. 
  //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Get_ttTRF_2018(string sample="data",string syst="nominal",string sel="5j6j",bool showall=false, bool protectLweights=false, string camp="mc16a",bool savetree=false)
double GetEffexpost(string sample="mc", int jmul=5,string camp="mc16a",string syst="ttPy8", int excl=0,string selincl="", int method=0,int ncopies=1,string parsyst="ptdRtj") {
  
  double result=0,mean=0,uncert=0;
  std::vector<double> results;
  string incl="";
  if (selincl.find("ge")!=string::npos) incl="ge";

  TChain*t3b = new TChain("nominal_Loose");
  TChain*t4b = new TChain("nominal_Loose");

  TChain*t3bdata = new TChain("nominal_Loose");
  TChain*t4bdata = new TChain("nominal_Loose");
 
  string channel = "ljets",folder = "/eos/infnts/atlas/gpanizzo/SM4t_L2/common-framework/";
  if (selincl.find("dilep")!=string::npos) channel="dilep";
  std::string parfilesuff= "allpar"; //"ptdRtj";
  
  if (sample == "mc"){
    if (syst.find("ttPy8")!=std::string::npos) {
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_pyt8_hdamp258" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_pyt8_hdamp258" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
    }
    else if (syst.find("ttPs")!=std::string::npos) {
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_her7_1l" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_her7_1l" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_her7_dil" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_her7_dil" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
    }
    else if (syst.find("ttGen")!=std::string::npos) {
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_amcatnlo_1l" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_amcatnlo_1l" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_amcatnlo_dil" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_amcatnlo_dil" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
    }
    else if (syst.find("ttRad")!=std::string::npos && syst.find("Up")!=std::string::npos) {
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_radHi_1l" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_radHi_1l" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_radHi_dil" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttbar_radHi_dil" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
    }
  }
  else if (sample=="data"){
      if (camp.find("mc16d") == std::string::npos){
        t3bdata->Add( Form( (folder + channel + incl + "%d" + "j/" + "data" + ".2015" + "." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
        t4bdata->Add( Form( (folder + channel + incl + "%d" + "j/" + "data" + ".2015" + "." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
        t3bdata->Add( Form( (folder + channel + incl + "%d" + "j/" + "data" + ".2016" + "." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
        t4bdata->Add( Form( (folder + channel + incl + "%d" + "j/" + "data" + ".2016" + "." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      }
      else {
        t3bdata->Add( Form( (folder + channel + incl + "%d" + "j/" + "data" + ".2017" + "." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
        t4bdata->Add( Form( (folder + channel + incl + "%d" + "j/" + "data" + ".2017" + "." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      }
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "wjets_sliced_221" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "wjets_sliced_221" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "zjets_sliced_221" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "zjets_sliced_221" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "singleTop" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "singleTop" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttW" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttW" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttZ" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttZ" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttWW" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttWW" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttH" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "ttH" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
      t3b->Add( Form( (folder + channel + incl + "%d" + "j/" + "diboson" + "." + camp +"." + parfilesuff + "_ttTRF_3b.root").c_str(), jmul) );
      t4b->Add( Form( (folder + channel + incl + "%d" + "j/" + "diboson" + "." + camp +"." + parfilesuff + "_ttTRF_ge4b.root").c_str(), jmul) );
  }
  
  TH1D*weightalljets   =   new TH1D("weightalljets",   "weightalljets",1,0,1);
  TH1D*weightpromotedjets   = new TH1D("weightpromotedjets", "weightpromotedjets",1,0,1);
  TH1D*epsest3b   =      new TH1D("epsest3b",      "epsest3b",1,0,1);

  TH1D*testweightpromotedjets=(TH1D*)weightpromotedjets->Clone("testweightpromotedjets");
  TH1D*testweightalljets=(TH1D*)weightalljets->Clone("testweightalljets");

  bool is2ex3ex = (method>0 and excl==0);
  string weightstr = "weight_normalise*weight_mc*weight_pileup*weight_leptonSF*weight_bTagSF_MV2c10_77*weight_jvt",weightsuff_base="1.0";

  if (sample=="data") {
     weightstr+="*(-1.0)"; // subtracting
     if	     (camp=="mc16a") weightstr+="* 36100";
     else if (camp=="mc16d") weightstr+="* (79.9-36.1)*1000";
     // given MC samples in data are weighted by lumi, there it makes sense to ask a single event doesn't count more than for 3 (I would say even not more than 1, but just to be conservative ...)
     weightstr+="* (TMath::Abs(" + weightstr+  ")<3  )";
  }
  if (channel=="dilep") weightsuff_base+="*(pass_sf_cut)";
  else weightsuff_base+="*(pass_triangle>0 )";


  cout << endl << "   ==> Computing effective efficiency e_"<< ((excl==0) ? "3b." : "ge4b.") << endl;
  cout  <<        "   ==>      Getting it from  "<< (is2ex3ex ? "Prob(3b)/[njtilde Prob(2b)]." : "ge4b.") << endl << endl;
  cout  << "                     Data is weighted by: "<< weightsuff_base << endl;
  cout << "                       MC is weighted by: "<< weightstr << endl << "                                           * "<< weightsuff_base << endl;


  // int ncopies =10;
  for (int icopies=0; icopies<ncopies; icopies++){
  string weightsuff=weightsuff_base;
  string  tttrfweight="weight_ttTRF", nBTagsStr="nBTags_MV2c10_77";
  if (parsyst!=""){
     string newparsyst="";
     if (parsyst.find("_")==0) newparsyst=parsyst;
     else newparsyst="_"+parsyst;
     tttrfweight+=newparsyst; 
     nBTagsStr+=newparsyst; 
  }
  cout << "                             ttTRF weight: "<< tttrfweight << endl;

  weightsuff+=Form("* (Entry$%s%d==%d)","%",ncopies,icopies); 
  cout  << "                 Running under subsample: "<< weightsuff << endl;

  weightpromotedjets->SetBinContent(1,0);
  weightpromotedjets->SetBinError(1,0);
  weightalljets->SetBinContent(1,0);
  weightalljets->SetBinError(1,0);


  // f3b, filled as "0" in case we are computing e_ge4b (excl=1):
  //
  //   *  Data
  t3bdata->Draw("0.5>>weightpromotedjets",     Form(   (weightsuff+"*"+tttrfweight+"*("+nBTagsStr+"-%d)*("+nBTagsStr+"==3)").c_str(),      2+excl), "goff"  );
  t3bdata->Draw("0.5>>testweightpromotedjets", Form(   (weightsuff+"*"+tttrfweight+"*("+nBTagsStr+"-%d)").c_str(),                         2+excl), "goff"  );
  if (weightpromotedjets->GetBinContent(1)!=testweightpromotedjets->GetBinContent(1)) cout << "WARNING: mismatch in ex-post efficiency computation (data). "<< endl;
  cout << "          Partial weight promoted (data): "<< weightpromotedjets->GetBinContent(1) << endl;

  //   *  Non-ttbar
  t3b->Draw(    "0.5>>+weightpromotedjets",    Form(     (weightstr+"*"+weightsuff+"*"+tttrfweight+"*("+nBTagsStr+"-%d)*("+nBTagsStr+"==3)").c_str(),  2+excl), "goff"  );
  t3b->Draw(    "0.5>>+testweightpromotedjets",Form(     (weightstr+"*"+weightsuff+"*"+tttrfweight+"*("+nBTagsStr+"-%d)").c_str(),                     2+excl), "goff"  );
  if (weightpromotedjets->GetBinContent(1)!=testweightpromotedjets->GetBinContent(1)) cout << "WARNING: mismatch in ex-post efficiency computation ( mc ). "<< endl;
  cout << "    Partial weight  promoted (non-ttbar): "<< weightpromotedjets->GetBinContent(1) << endl;

  // (nj-2) f2b, filled as "0" in case we are computing e_ge4b (excl=1):
  t3bdata->Draw("0.5>>weightalljets",        Form( (              weightsuff+"*(nJets-%d)*("+nBTagsStr+"-%d)*("+nBTagsStr+"==3)*("+ tttrfweight +">0)").c_str(), 2+excl,2+excl), "goff" ); // 2b
  cout << "               Partial weight all (data): "<< weightalljets->GetBinContent(1) << endl;
  t3b->Draw(    "0.5>>+weightalljets",       Form( (weightstr+"*"+weightsuff+"*(nJets-%d)*("+nBTagsStr+"-%d)*("+nBTagsStr+"==3)*("+ tttrfweight +">0)").c_str(), 2+excl,2+excl), "goff" ); // 2b
  cout << "         Partial weight  all (non-ttbar): "<< weightalljets->GetBinContent(1) << endl;

  if (!is2ex3ex) {
     // We just need to add remainder terms;
     //
     t4bdata->Draw("0.5>>+weightpromotedjets", Form( (               weightsuff+"*"+tttrfweight+"*("+nBTagsStr+"-%d)*("+nBTagsStr+">3)").c_str(),    2+excl), "goff" );
     t4b->Draw(    "0.5>>+weightpromotedjets", Form( (weightstr +"*"+weightsuff+"*"+tttrfweight+"*("+nBTagsStr+"-%d)*("+nBTagsStr+">3)").c_str(),    2+excl), "goff" );
 
     t3bdata->Draw("0.5>>+weightalljets",   Form( (              weightsuff+"*"+tttrfweight+"* (nJets-%d)").c_str(),     2+excl), "goff" );     // 3b
     t3b->Draw(    "0.5>>+weightalljets",   Form( (weightstr+"*"+weightsuff+"*"+tttrfweight+"* (nJets-%d)").c_str(),     2+excl), "goff" );     // 3b
     t4bdata->Draw("0.5>>+weightalljets",   Form( (              weightsuff+"*"+tttrfweight+"* (nJets-%d)").c_str(),     2+excl), "goff" );     // ge4b
     t4b->Draw(    "0.5>>+weightalljets",   Form( (weightstr+"*"+weightsuff+"*"+tttrfweight+"* (nJets-%d)").c_str(),     2+excl), "goff" );     // ge4b
  }

  epsest3b->Divide(weightpromotedjets,weightalljets);
  result = epsest3b->GetBinContent(1);
  if (is2ex3ex) result = result/(1.0+result);

  cout << "     ---------------------------------------------------------" << endl;
  cout << "                  Total weight  promoted: "<< weightpromotedjets->GetBinContent(1) << endl;
  cout << "                       Total weight  all: "<< weightalljets->GetBinContent(1) << endl;
  cout << "     ---------------------------------------------------------" << endl;

  mean+= result;
  uncert+=result*result;
  results.push_back(result);
  }

  mean/=ncopies;
  uncert = sqrt((uncert/ncopies - mean*mean ) / ncopies);

  TFile*fmap;
  string jetsuff="j";
  if (channel=="dilep") jetsuff+="_dilep";
  if (excl) fmap=TFile::Open( Form( ("data/ttTRFmaps2018_" + sample + "_" + camp + "_" + incl + "%d" + jetsuff + "_4b__" + syst + ".root").c_str(), jmul)  );
  else      fmap=TFile::Open( Form( ("data/ttTRFmaps2018_" + sample + "_" + camp + "_" + incl + "%d" + jetsuff + "__" + syst + ".root").c_str(), jmul)  );
  TH1D*h_eff = (TH1D*) fmap->Get("h_eff");
  
  int ndigits = floor(TMath::Abs(TMath::Log10(TMath::Abs(h_eff->GetBinError(1)))));
  cout << setprecision(ndigits +2) << std::fixed;

  cout << "            Efficiency estimated ex post: " << mean << "+- " << uncert << endl;
  cout << "        Efficiency as computed from data: " <<  h_eff->GetBinContent(1)  << " +- " << h_eff->GetBinError(1) << endl << endl;

  if (ncopies>2){
     cout << setprecision(ndigits +3) << std::fixed;
     std::cout << "Efficiencies:" << endl;
     for (int ic=0; ic<ncopies; ic++){
         std::cout <<  results[ic] << (ic!=ncopies-1 ? ", " : "");
     }
     std::cout << endl << "Residues:" << endl;
     for (int ic=0; ic<ncopies; ic++){
         std::cout <<  (results[ic] - mean)/(uncert*sqrt(ncopies*1.0)) << (ic!=ncopies-1 ? ", " : "");
     }
     cout << endl;
  }


  delete t3b;
  delete t4b;
  delete weightalljets;
  delete weightpromotedjets;
  delete epsest3b; 
  fmap->Close();
  return mean;
}
