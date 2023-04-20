#include "TtTRF/TtTRFapplication.h"

TtTRFapplication::TtTRFapplication(int ntags,std::string path,std::string sample,std::string systematic,std::string selection,std::string splitting,std::string parametrisation,bool debug){  // sample = data or mc, syst = nominal, ...
  m_debug = debug;
  m_ntags = ntags;
  sel = selection;
  std::string syst = systematic;
  m_split3b = splitting.find("3b")!=std::string::npos;
  m_split4b = splitting.find("4b")!=std::string::npos;
  m_incltoexcl = splitting.find("itoe")!=std::string::npos; // e_ge2b(2excl->3incl);e_ge3b(3excl/4incl) instead of e_ge2b(2excl->3excl);e_ge3b(3excl/4incl)

  SetParametrisations(parametrisation);
  
  if(m_debug){
    if(m_split3b)                std::cout << "Opening file " << path+"/ttTRFmaps2018_"+sample+"_"+sel+"_3b__"+syst+".root" << std::endl;
    if(m_split4b)                std::cout << "Opening file " << path+"/ttTRFmaps2018_"+sample+"_"+sel+"_4b__"+syst+".root" << std::endl;
    if(!m_split3b || !m_split4b) std::cout << "Opening file " << path+"/ttTRFmaps2018_"+sample+"_"+sel+"__"   +syst+".root" << std::endl;
  }
  
  for ( auto sparam : m_parametrisations ){
    m_nBtags_ttTRF[sparam] = 2;
    string sparam3b=sparam;

    // FIXME: leave the user the possibility of applying different maps/corrections when 2be->3be->4bi ? Only if -strongly- motivated!
    // if (m_split4b && m_ntags>=4 ){    // do not overcorrect when taking into account about evolution vs Nj
    //  size_t spos = sparam3b.find("Njdist");
    //  if (spos!=std::string::npos) sparam3b.replace(spos,6,"");
    // }

    if(m_split3b) m_TRFreader0[sparam] = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"_3b__"+syst+".root",sparam3b) ;
    else          m_TRFreader0[sparam] = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"__"   +syst+".root",sparam3b) ;
    if(m_split4b) m_TRFreader1[sparam] = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"_4b__"+syst+".root",sparam) ;
    else          m_TRFreader1[sparam] = new TtTRFreader(path+"/ttTRFmaps2018_"+sample+"_"+sel+"__"   +syst+".root",sparam) ;

  //   // create other instances for weight systematics, only in case of nominal tree
  //   if(syst=="nominal"){
  //     for(auto systName : m_systNames){
  //       if(m_debug) std::cout << "Opening " << "ttTRFmaps_"+sample+"_"+sel+"__"   +systName+".root" << std::endl;
  //       m_systTRFreader0.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"__"   +systName+".root"));
  //       m_systTRFreader1.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"_4b__"+systName+".root"));
  //     }
  //     for(unsigned int i_syst=0;i_syst<m_systVecNames.size();i_syst++){
  //       for(unsigned int ii_syst=0;ii_syst<m_weight_systVec[i_syst].size();ii_syst++){
  //         std::string systName = systVecNames[i_syst]+Form("%d",ii_syst);
  //         systTRFreader0.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"__"   +systName+".root"));
  //         systTRFreader1.push_back(new TtTRFreader("ttTRFmaps_"+sample+"_"+sel+"_4b__"+systName+".root"));
  //       }
  //     }
  //   }
  
    // 2i->3e/3i
    m_TRF[sparam] = new CustomTRF();
    m_TRF[sparam]->AddWorkingPoint("mv2c10_77");
  //   std::vector<CustomTRF*> m_systTRF;
  //  for(auto trfReader : m_systTRFreader0){
  //     m_systTRF.push_back(new CustomTRF());
  //     m_systTRF[systTRF.size()-1]->AddWorkingPoint("mv2c10_77");
  //   }

    // 2i->2e
    m_TRF0[sparam] = new CustomTRF();
    m_TRF0[sparam]->AddWorkingPoint("mv2c10_77");
  //   std::vector<CustomTRF*> m_systTRF0;
  //   for(auto trfReader : m_systTRFreader0){
  //     m_systTRF0.push_back(new CustomTRF());
  //     m_systTRF0[systTRF0.size()-1]->AddWorkingPoint("mv2c10_77");
  //   }
  
    // 3i->4i
    m_TRF1[sparam] = new CustomTRF();
    m_TRF1[sparam]->AddWorkingPoint("mv2c10_77");
  //   std::vector<CustomTRF*> m_systTRF1;
  //   for(auto trfReader : m_systTRFreader1){
  //     m_systTRF1.push_back(new CustomTRF());
  //     m_systTRF1[systTRF1.size()-1]->AddWorkingPoint("mv2c10_77");
  //   }
    
    // 3i->3e
    m_TRF2[sparam] = new CustomTRF();
    m_TRF2[sparam]->AddWorkingPoint("mv2c10_77");
  //   std::vector<CustomTRF*> m_systTRF2;
  //   for(auto trfReader : m_systTRFreader1){
  //     m_systTRF2.push_back(new CustomTRF());
  //     m_systTRF2[systTRF2.size()-1]->AddWorkingPoint("mv2c10_77");
  //   }
  }
}

// TtTRFapplication::AddJet(int idx,float pt,float eta,float phi,float e,float weight,int istagged){
void TtTRFapplication::AddJet(int idx,float pt,float eta,float phi,float e,int istagged){
  m_jet_index.push_back(idx);
  TLorentzVector jet(0,0,0,0);
  jet.SetPtEtaPhiE(pt,eta,phi,e);
  m_jet.push_back(jet);
//   m_jet_weight.push_back(weight);
  m_jet_istagged.push_back(istagged);
}

void TtTRFapplication::Reset(){
  m_jet_index.clear();
  m_jet.clear();
//   m_jet_weight.clear();
  m_jet_istagged.clear();
  m_jet_dRjmin.clear();
  for (auto sparam : m_parametrisations) {
      m_weight[sparam] =0;
  }
}

void TtTRFapplication::Evaluate(){
  int nJets = (int)m_jet.size();
  if(m_debug) std::cout << "---- * * * ---\n" << "nJets = " << nJets << std::endl;
  //
  // get the first two tagged jets (there should be just two of them)
  int j1st = -1;
  int j2nd = -1;
  for(int j=0;j<nJets;j++){
    if(m_jet_istagged[j]){
      if(j1st<0)      j1st = j;
      else if(j2nd<0) j2nd = j;
      else{
        if(m_debug) 
          std::cout << "ttTRF::TtTRFapplication: WARNING: Trying to apply ttTRF to an event with > 2 b-tagged jets. Returning weight = 0." << std::endl;
        for ( auto sparam : m_parametrisations ) m_weight[sparam] = 0.;
        return;
      }
    }
  }
  // G.
  if (j1st<0 || j2nd < 0) {
       for ( auto sparam : m_parametrisations ) m_weight[sparam] =0.;
       if(m_debug) 
          std::cout << "ttTRF::TtTRFapplication: WARNING: not enough btagged jets found, returning weight = 0." << std::endl;
       // save correct direct nBTags, sparing storage space (events will be skipped when requiring nBTags >= 2)
       if (j1st*j2nd>0) for ( auto sparam : m_parametrisations ) m_nBtags_ttTRF[sparam]=0;
       else for ( auto sparam : m_parametrisations ) m_nBtags_ttTRF[sparam]=1;
       return;
  }
  //
  for ( auto sparam : m_parametrisations ){
    m_TRF[sparam]->Reset();
    m_TRF0[sparam]->Reset();
    m_TRF1[sparam]->Reset();
    m_TRF2[sparam]->Reset();
  }
  //

  // Bulk jets variables
  float dRj_bulk=m_jet[j1st].DeltaR(m_jet[j2nd]);

  //
  int jj = 0;
  float dRmin_all = 1000;
  for(int j=0;j<nJets;j++){
    if(j == j1st || j == j2nd) {
             m_jet_dRjmin.push_back(-999);
             continue;
    }
    // find min dR within all jets
    ////////////////////////////
    if (sel.find("nofix")==std::string::npos) dRmin_all=100;
    ////////////////////////////
    for(int jjj=0;jjj<nJets;jjj++){
      if(j==jjj) continue;
      float dRj = m_jet[j].DeltaR(m_jet[jjj]);
      if(dRj<dRmin_all){
        dRmin_all = dRj;
      }
    }
    m_jet_dRjmin.push_back(dRmin_all);
    //
    float jetPt = m_jet[j].Pt();
    float jetEta = TMath::Abs(m_jet[j].Eta());
    if(m_debug) std::cout << std::endl<< "..jet " << jj << " with pT "<< jetPt << ";  " ;

    float dRmin_Nj_tilde = 5*0.38 + (dRmin_all-0.38) * (5 + (nJets-5) * 0.3083);
    // dRmin_Nj_tilde = dRmin_all * nJets;

    float btageff;
    for ( auto sparam : m_parametrisations ){
      btageff = m_TRFreader0[sparam]->GetEff(jetPt,jetEta,dRmin_Nj_tilde,nJets,dRj_bulk);
      if(m_debug) std::cout << "-> btageff("<< sparam << ") = " << btageff << std::endl << "    " << "   " << "         "<< "    "<< "   ";
      m_TRF0[sparam]->SetJetBtagEff(jj,0,btageff);
      m_TRF[sparam] ->SetJetBtagEff(jj,0,btageff);
    }
    //
    jj ++;
  }
  for ( auto sparam : m_parametrisations ){
    if(m_ntags>=4 && !m_split4b) m_TRF[sparam] ->SetBtagRequirement(m_ntags-2,true,0);
    else  if (m_incltoexcl)      m_TRF[sparam] ->SetBtagRequirement(1,true,0);
          else                   m_TRF[sparam] ->SetBtagRequirement(1,false,0);
    m_TRF0[sparam]->SetBtagRequirement(0,false,0);
    m_TRF[sparam] ->Evaluate();
    m_TRF0[sparam]->Evaluate();
    if(m_debug) std::cout << "-------\n" << "Nominal(" << sparam  << "): " << m_TRF[sparam]->GetTRFweight() << " " << m_TRF0[sparam]->GetTRFweight() << std::endl;
    m_weight[sparam] = m_TRF[sparam]->GetTRFweight() / (double) m_TRF0[sparam]->GetTRFweight();
    if(m_debug) std::cout << " -> "<< sparam << ": " << m_weight[sparam] << std::endl;
  }
  //
  // ...
  std::map<std::string, int> j3rd;
  for ( auto sparam : m_parametrisations ) j3rd[sparam] = -1;

  if ( (m_ntags>=4 || (m_ntags==3 && m_incltoexcl)) && m_split4b ){
    jj = 0;
    int jjjj = 0;

    // find j3rd before, allowing incltoecxl to work properly
    // Note that in case only one jet has been promoted to be b-tagged
    // this choice is equivalent to just pick that only one (default when m_incltoexcl=false) 
    float thisbtageff;
    std::map<std::string, float> maxbtageff;
    for ( auto sparam : m_parametrisations ) maxbtageff[sparam] = -10;

    
    for(int j=0;j<nJets;j++){
      // skip the usual two real b-tags
      if(j == j1st || j == j2nd) continue;
      jjjj++;
      float jetPt = m_jet[j].Pt();
      float jetEta = TMath::Abs(m_jet[j].Eta());
      dRmin_all=m_jet_dRjmin[j];

      float dRmin_Nj_tilde = 5*0.38 + (dRmin_all-0.38) * (5 + (nJets-5) * 0.3083);
      // dRmin_Nj_tilde = dRmin_all * nJets;

      // find 3rd excluded jet, maybe different for each different parametrisation
      for ( auto sparam : m_parametrisations ){
        thisbtageff = m_TRFreader0[sparam]->GetEff(jetPt,jetEta,dRmin_Nj_tilde,nJets,dRj_bulk);
        if(m_TRF[sparam]->GetJetIsTagged(jjjj-1,0) && thisbtageff>maxbtageff[sparam]){
          j3rd[sparam] = j;
          maxbtageff[sparam] = thisbtageff;
        }
      }
    }
 
    int null=0, tot=0;
    for ( auto sparam : m_parametrisations ) {
      if ( j3rd[sparam] < 0) {
          m_weight[sparam] =0.;
          if(m_debug)
             std::cout << "ttTRF::TtTRFapplication: WARNING: 3rd promoted btagged jet not found, returning weight(" << sparam << ") = 0." << std::endl;
          m_nBtags_ttTRF[sparam]=2;
          // return;
          null++;
      }
      tot++;
    }
    if (null==tot) return;


    for ( auto sparam : m_parametrisations ) {
      if ( j3rd[sparam] < 0) continue;    

      // update bulk jet variables given we have now j3rd
      float dRj_bulk_ge4b = ( dRj_bulk  +  m_jet[j1st].DeltaR(m_jet[j3rd[sparam]]) + m_jet[j2nd].DeltaR(m_jet[j3rd[sparam]]) ) / 3.0;


      dRmin_all = 1000;
      jjjj=0; // going to keep trace of index of jets not excluded by the first step (i.e. all but j1st and j2nd) 
      jj=0;   // going to keep trace of index of jets not excluded by the second step (i.e. all but j1st and j2nd and j3rd) 
      for(int j=0;j<nJets;j++){
        // skip the usual two real b-tags
        if(j == j1st || j == j2nd) continue;
        jjjj++;
        // skip the "promoted" jet as well
        /*  if(m_TRF->GetJetIsTagged(jjjj-1,0)){
            j3rd = j;
            continue;
          }
        */
        if (j == j3rd[sparam]) continue;
        //
        // find min dR within all jets
        ////////////////////////////
        if (sel.find("nofix")==std::string::npos) dRmin_all=100;
        ////////////////////////////
        for(int jjj=0;jjj<nJets;jjj++){
          if(j==jjj) continue;
          float dRj = m_jet[j].DeltaR(m_jet[jjj]);
          if(dRj<dRmin_all){
            dRmin_all = dRj;
          }
        }
        if (dRmin_all != m_jet_dRjmin[j]) cout << "WARNING: TtTRFapplication mismatch in variables computation !" << endl;
        //
        float jetPt = m_jet[j].Pt();
        float jetEta = TMath::Abs(m_jet[j].Eta());

        float dRmin_Nj_tilde = 5*0.38 + (dRmin_all-0.38) * (5 + (nJets-5) * 0.3083);
        // dRmin_Nj_tilde = dRmin_all * nJets;

        float btageff = m_TRFreader1[sparam]->GetEff(jetPt,jetEta,dRmin_Nj_tilde,nJets,dRj_bulk_ge4b);
        m_TRF1[sparam]->SetJetBtagEff(jj,0,btageff);
        m_TRF2[sparam]->SetJetBtagEff(jj,0,btageff);
        //
        jj ++;
      }
    //
  //     m_TRF1->SetBtagRequirement(m_ntags-3,incl,0);

      m_TRF2[sparam]->SetBtagRequirement(0,        false,0);
      m_TRF1[sparam]->SetBtagRequirement(m_ntags-3,true, 0);
      m_TRF1[sparam]->Evaluate();
      m_TRF2[sparam]->Evaluate();
      if(m_debug) std::cout << "-------\n" << "Nominal("<<sparam<<"), 2nd step: " << m_TRF1[sparam]->GetTRFweight() << " " << m_TRF2[sparam]->GetTRFweight() << std::endl;
      if (m_incltoexcl) {
           if (m_ntags==3)  m_weight[sparam] *= m_TRF2[sparam]->GetTRFweight();
           else             m_weight[sparam] *= m_TRF1[sparam]->GetTRFweight();
      }
      else                  m_weight[sparam] *= m_TRF1[sparam]->GetTRFweight() / (double) m_TRF2[sparam]->GetTRFweight();
      if(m_debug) std::cout << " -> "<< sparam << ": " << m_weight[sparam] << std::endl;
      //
    }
  }

  // save which jet is tagged:
  for ( auto sparam : m_parametrisations ) {
    jj = 0;
    m_jet_istagged_ttTRF[sparam].clear();
    if ( (m_ntags>=4 || (m_ntags==3 && m_incltoexcl)) && m_split4b ) if ( j3rd[sparam] < 0) continue;
    for(int j=0;j<nJets;j++){
      m_jet_istagged_ttTRF[sparam].push_back(false);
      if(j==j1st){ m_jet_istagged_ttTRF[sparam][j] = true; if(m_debug) std::cout << j << " is j1st  " << std::endl; continue; }
      if(j==j2nd){ m_jet_istagged_ttTRF[sparam][j] = true; if(m_debug) std::cout << j << " is j2nd  " << std::endl; continue; }
      if (m_incltoexcl) {
        if(m_ntags>=3 && j==j3rd[sparam]){ m_jet_istagged_ttTRF[sparam][j] = true; if(m_debug) std::cout << j << " is j3rd  " << std::endl; continue; }
      }
      else {
        if(m_ntags>=4 && j==j3rd[sparam]){ m_jet_istagged_ttTRF[sparam][j] = true; if(m_debug) std::cout << j << " is j3rd("<< sparam<<")  " << std::endl; continue; }
        if(m_ntags<=3){ if(m_debug) m_TRF[sparam]->PrintJets(); if(m_TRF[sparam] ->GetJetIsTagged(jj,0)) { m_jet_istagged_ttTRF[sparam][j] = true; if(m_debug) std::cout << j << " tagged by m_TRF("<< sparam <<") " << std::endl;}}
      }
      if(m_ntags>=4 && m_split4b){ if(m_debug) {m_TRF1[sparam]->PrintJets(); std::cout << "Trying to access jet "<<jj<<" in TRF1."<< std::endl;}; if( m_TRF1[sparam]->GetJetIsTagged(jj,0)) {m_jet_istagged_ttTRF[sparam][j] = true; if(m_debug) std::cout << j << " tagged by m_TRF1 " << std::endl; }}
      if(m_ntags>=4 && !m_split4b){ if(m_debug) {m_TRF[sparam]->PrintJets(); std::cout << "Trying to access jet "<<jj<<" in TRF."<< std::endl;}; if( m_TRF[sparam]->GetJetIsTagged(jj,0)) {m_jet_istagged_ttTRF[sparam][j] = true; if(m_debug) std::cout << j << " tagged by m_TRF " << std::endl; }}
      jj++;
    }
    //
    if(m_ntags<=3) m_nBtags_ttTRF[sparam] = m_TRF[sparam] ->GetNumBtagJets(0)+2;
    if(m_ntags==3 && m_incltoexcl && m_split4b) m_nBtags_ttTRF[sparam] = m_TRF2[sparam] ->GetNumBtagJets(0)+3;
    if( m_ntags>=4 && m_split4b)  m_nBtags_ttTRF[sparam] = m_TRF1[sparam]->GetNumBtagJets(0)+3;  
    if(m_ntags>=4 && !m_split4b) m_nBtags_ttTRF[sparam] = m_TRF[sparam] ->GetNumBtagJets(0)+3;  
    //
  }
}


float TtTRFapplication::GetTRFweight(std::string namepar){
  return (float) m_weight[namepar];
}

float TtTRFapplication::GetTRFweight(){
  return (float) m_weight[m_parametrisations[0]];
}

bool TtTRFapplication::GetJetIsTagged(int idx, std::string namepar){
  if((int)m_jet_istagged_ttTRF[namepar].size()<=idx) return false;
  return m_jet_istagged_ttTRF[namepar][idx];
}

bool TtTRFapplication::GetJetIsTagged(int idx){
  return GetJetIsTagged(idx, m_parametrisations[0]);
}

int TtTRFapplication::GetNBTags(){
  return m_nBtags_ttTRF[m_parametrisations[0]];
}

int TtTRFapplication::GetNBTags(std::string namepar){
  return m_nBtags_ttTRF[namepar];
}


float TtTRFapplication::GetJetVar(int idx, string Var){

   if (Var.find("dRjmin")!=std::string::npos) {
      if((int)m_jet_dRjmin.size()<=idx) return -999;
      return m_jet_dRjmin[idx];
   }
   else return -999;
}

void TtTRFapplication::SetParametrisations(std::string param){

  size_t first=0,last;
  do {
    last = param.find(",",first);
    if (last==std::string::npos) last = param.size();
    m_parametrisations.push_back( param.substr(first,last-first) ); 
    first=last+1;
  } while (last<param.size());
}

void TtTRFapplication::GetParametrisations(std::vector<std::string> * param){
    for ( auto sparam : m_parametrisations ) param->push_back(sparam);
}
