#include <vector>
#include <iostream>
#include <map>

#include "TMath.h"
#include "TF1.h"
#include "TH1D.h"
#include "TFile.h"
#include "TKey.h"

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

void add_normalisation(TString filename, bool isSyst=false){
  std::cout << filename << " " << isSyst << std::endl;

  TFile *file = new TFile(filename, "update");

  TTreeReader reader("sumWeights", file);
  TTreeReaderValue<vector<float>> mcweights(reader, "totalEventsWeighted_mc_generator_weights");
  TTreeReaderValue<float> xsec(reader, "xsec");
  TTreeReaderValue<int> dsid(reader, "dsid");

  std::map<int, float> norm_per_dsid;
  std::map<int, vector<float>> norms_per_dsid;

  float sm4top_sig_xsec = 0.012;
  std::map<int, float> norm_sig_xsec =
    {
     {312440,0.023880},
     {312441,0.049260},
     {312442,0.025740},
     {312443,0.011180},
     {312444,0.009070},
     {312445,0.007091},
     {312446,0.003601}
    };

  // loop over sumWeights
  while(reader.Next()){

    // sum of nominal weights
    if( norm_per_dsid.find(*dsid) == norm_per_dsid.end() ) norm_per_dsid[*dsid] = 0;
    norm_per_dsid[*dsid] += (*mcweights).at(0)/(*xsec);

    // sum of internal weights
    if(!isSyst){
      for(unsigned int iw=0; iw<(*mcweights).size(); ++iw){
	if( iw >= norms_per_dsid[*dsid].size() ) 
	  norms_per_dsid[*dsid].push_back( (*mcweights).at(iw)/(*xsec) );
	else 
	  norms_per_dsid[*dsid].at(iw) += (*mcweights).at(iw)/(*xsec);
      }
    }

  }

  // Get the inverse of all normalization
  for( auto &norm : norm_per_dsid )
    norm.second = 1./norm.second;

  if(!isSyst){
    for ( auto &norms : norms_per_dsid ) {
      for( unsigned int iw=0; iw<norms.second.size(); ++iw)
	norms.second.at(iw) = 1./norms.second.at(iw);
    }
  }

  // Print results
  for ( const auto &norm : norm_per_dsid ) {
    std::cout << "DSID " << norm.first << ": " << norm.second << std::endl;
    // for( unsigned int iw=0; iw<norms.second.size(); ++iw)
    //   std::cout << norms.second.at(iw) << std::endl;
  }

  // Extract the list of trees
  std::vector<TString> list_trees;
  TList *list = file->GetListOfKeys();
  TIter iter(list->MakeIterator());
  while(TObject* obj = iter()){
    TKey* theKey = (TKey*)obj;
    if ( !TString (theKey->GetClassName() ).EqualTo ("TTree")) continue;
    if ( TString (theKey->GetName() ).EqualTo ("sumWeights")) continue;
    list_trees.push_back(theKey->GetName());
  }

  // Looping over trees in the input file
  for(TString tree_name: list_trees){
    std::cout << tree_name << std::endl;
    TTree * tree= (TTree*) file->Get(tree_name);
    if (tree==0x0) {
      std::cerr << "ERROR Tree " << tree_name << " empty (0x0), skipping" << std::endl;
      continue;
    }    

    tree->SetBranchStatus("*",1);

    if(tree->GetListOfBranches()->FindObject("weight_normalise")) {
      std::cout << "weight_normalise already exist. Skipping." << std::endl;
      continue;
    }

    UInt_t mcChannelNumber = 0;
    tree->SetBranchAddress ("mcChannelNumber", &mcChannelNumber );

    float weight_normalise = 0;
    TBranch *b_weight_normalise = tree->Branch("weight_normalise", &weight_normalise, "weight_normalise/F");

    vector<float> weight_mcweight_normalise;
    TBranch *b_weight_mcweight_normalise = 0;
    if(!isSyst) b_weight_mcweight_normalise = tree->Branch("weight_mcweight_normalise", &weight_mcweight_normalise);

    int nentries = tree->GetEntries();
    for (int i = 0; i < nentries; i++){
      tree->GetEntry(i);      

      float weight_signal_normalise = norm_sig_xsec[mcChannelNumber]>1.e-06?sm4top_sig_xsec/norm_sig_xsec[mcChannelNumber]:1.0;
      
      weight_normalise = norm_per_dsid[mcChannelNumber]*weight_signal_normalise;
      if(!isSyst) weight_mcweight_normalise = norms_per_dsid[mcChannelNumber];

      b_weight_normalise->Fill();
      if(!isSyst) b_weight_mcweight_normalise->Fill();
    }

    tree->Write("", TObject::kOverwrite);
    delete tree;
  } //loop of trees

  file->Close();
  delete file;
}
