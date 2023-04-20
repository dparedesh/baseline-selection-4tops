#ifndef _TTHBB_ADDBDT1LOS_H_
#define _TTHBB_ADDBDT1LOS_H_

#include "TTHbbToolManager/ToolBase.h"
#include "TTHbbObjects/Event.h"
#include "TTHbbClassification/ClassBDTCalculatorTool.h"
#include "TTHbbMVAInterfaces/TMVAConfiguration.h"
#include "MVAVariables/MVAVariables.h"

namespace TTHbb {
  class AddBDT1LOS : public ToolBase {

  public:
    AddBDT1LOS(){};
    ~AddBDT1LOS(){};

    void initialise();
    void apply(TTHbb::Event* event);
    void finalise();

  private:
    
    std::string m_xmlPath_1l;
    std::string m_xmlPath_2l;

    bool m_run1l;
    bool m_run2l;

    mutable ClassBDTCalculatorTool m_classBDTCalculatorTool;

    std::vector<std::string> wps;

    void  GetOrderedMV2Vars(std::shared_ptr<MVAVariables> mva, TTHbb::Event* event);
    void  GetOrderedDL1rVars(TTHbb::Event* event);
    float GetFloatedPCB(float dl1r);
    float Get_Mjjj_MindR(TTHbb::Event* event);
    float Get_Mjjj_AvgdRs3(TTHbb::Event* event);
    
    TLorentzVector GetTlorentzVector(std::shared_ptr<Jet> jet);

  };
}

#endif

