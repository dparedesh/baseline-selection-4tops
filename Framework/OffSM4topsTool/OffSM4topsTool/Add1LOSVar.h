
#ifndef _ADDGLOBALVAR_H_
#define _ADDGLOBALVAR_H_

#include "TTHbbToolManager/ToolBase.h"
#include "TTHbbObjects/Event.h"

#include "TLorentzVector.h"

#include <string>
#include <vector>

namespace TTHbb {

  class Add1LOSVar : public ToolBase {
    /**
     * @brief Tool for postproduction of 1L/2LOS mini-ntuples
     * Compute additionnal global variables (e.g. nRCJets, mJsum, mll, mtw, SR).
     */

  public:
    /// Constructor
    Add1LOSVar(){};

    /// Destructor
    ~Add1LOSVar(){};

    /// Function to initialise objects
    void initialise();

    /// Fill new variables
    void apply(TTHbb::Event* event);

    /// Function to finalise
    void finalise();


  private:

    /// Function to check the signal region categorization
    int   signalRegion(TTHbb::Event* event);
    int   Year(TTHbb::Event* event);
    float WeightLumi(TTHbb::Event* event);
    void  Convert(TTHbb::Event* event);

    TLorentzVector GetTlorentzVector(std::shared_ptr<Jet> jet);
    TLorentzVector GetTlorentzVector(std::shared_ptr<Lepton> lep);
    TLorentzVector GetTlorentzVector(std::shared_ptr<ReclusteredJet> rcjet);
    
    void AddRCJetMass(TTHbb::Event* event);
    void Get_dPhi_met_lep_vars(TTHbb::Event* event);
    void Get_met_dilep_vars(TTHbb::Event* event);
    float Get_ptW(TTHbb::Event* event);
    
    /// Private parameter 
    bool isData;

  };
}

#endif
