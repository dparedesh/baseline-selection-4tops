/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "OffSM4topsTool/OffSM4topsConfiguration.h"
#include "PathResolver/PathResolver.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

namespace TTHbb{

  OffSM4topsConfiguration* OffSM4topsConfiguration::m_instance_4tops = 0;


  void  OffSM4topsConfiguration::AddSM4topsConfiguration() {
    /// Name defined for debugging
    m_name = "SM4t";

    /// List of settings needs to be added here for global config

    /// Settings for tools included in OffSM4tops (on top of OfflineTTHbb ones)

    registerSetting("Add1LOSVar.TurnOn","Tool to include variables for 1LOS channel",
                    "false"
                    );
    registerSetting("Add1LOSVar.AddHigherLevelVariables","Add new variables that are more elaborated ",
                    "false"
                    );
    registerSetting("Add1LOSVar.MeVToGeV","Decorate jet events variables in GeV (and change HT_jets) ",
                    "false"
                    );

    registerSetting("AddBDT1LOS.TurnOn", "Tool to include the BDT score", "false");
    registerSetting("AddBDT1LOS.XMLpath_1l", "Path to the 1l BDT XML files", "");
    registerSetting("AddBDT1LOS.Prefix_1l", "Prefix of the 1l BDT XML files", "TMVA_1l_4tops");
    registerSetting("AddBDT1LOS.Vars_1l", "List of variables used by the 1l BDT separated by commas",
		    "HT_jets,mJSum,nBTags_DL1r_77/I,nJets/I,nRCJets/I");
    registerSetting("AddBDT1LOS.XMLpath_2l", "Path to the 2l BDT XML files", "");
    registerSetting("AddBDT1LOS.Prefix_2l", "Prefix of the 2l BDT XML files", "TMVA_2l_4tops");
    registerSetting("AddBDT1LOS.Vars_2l", "List of variables used by the 2l BDT separated by commas",
		    "HT_jets,mJSum,nBTags_DL1r_77/I,nJets/I,nRCJets/I");

    registerSetting("AddSSMLVar.TurnOn","Please briefly describe the usage of this setting",
                    "false"
                    );

    registerSetting("AddSSMLVar.QmisIDPath","Location of the charge misID rates",
                    ""
                    );

    registerSetting("AddSSMLVar.doChargeFlip","Please briefly describe the usage of this setting",
                    "false"
                   );
    registerSetting("CommonTools.TurnOn", "A selection for tools used in common by SSML and 1LOS.", "false");

    registerSetting("TtTRF.TurnOn",
    		    "Set to true if the ttTRF tool should be used for data-driven ttbar+jets estimation",
    		    "false"
    		    );
    registerSetting("TtTRF.nBTags",
                    "Number of b-tags to reweight to for ttTRF: 3b or ge4b ",
                     "ge4b");
    registerSetting("TtTRF.Path",
                    "Location of ttTRF efficiencies ",
                    "../../TRF_evaluation");
    registerSetting("TtTRF.Sample",
                    "data or mc ",
                    "data_mc16a");
    registerSetting("TtTRF.Selection",
                    "Selection for ttTRF eff extraction: 5j, 5j6j... (SM4t)",
                    "5j");
    registerSetting("TtTRF.Debug",
                    "Debug for ttTRF ",
                    "false");
    registerSetting("TtTRF.Systematic", "Systematic for ttTRF eff extraction: default is nominal ", "nominal");
    registerSetting("TtTRF.Splitting", "Splitting of ttTRF eff: none, split3b4b, split3b, split4b ", "split3b4b");
    registerSetting("TtTRF.Parametrisation", "Comma separated list of parametrisations for ttTRF eff: pt, pt_eta_dRj, pt_dRj... ", "pt/dRj");


  }

OffSM4topsConfiguration* OffSM4topsConfiguration::get() {
  if (!m_instance_4tops) {
    m_instance_4tops = (OffSM4topsConfiguration*)GlobalConfiguration::get();
    m_instance_4tops->AddSM4topsConfiguration();
  }
  return m_instance_4tops;
}

}  // namespace TTHbb
