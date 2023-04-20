#include "OffSM4topsTool/OffSM4topsToolDictionary.h"
#include "OffSM4topsTool/Add1LOSVar.h"
#include "OffSM4topsTool/AddBDT1LOS.h"
#include "OffSM4topsTool/AddSSMLVar.h"
#include "OffSM4topsTool/CommonTools.h"
#include "OffSM4topsTool/TtTRF.h"

void TTHbb::OffSM4topsToolDictionary::buildDictionary(TTHbb::ToolManager* toolManager) {

  toolManager->addToolToDict("Add1LOSVar.TurnOn", "Add1LOSVar", [] { return new Add1LOSVar(); });

  toolManager->addToolToDict("AddBDT1LOS.TurnOn", "AddBDT1LOS", [] { return new AddBDT1LOS(); });

  toolManager->addToolToDict("TtTRF.TurnOn", "TtTRF", [] { return new TtTRF("TtTRF"); });

  toolManager->addToolToDict("AddSSMLVar.TurnOn", "AddSSMLVar", [] { return new AddSSMLVar(); });

  toolManager->addToolToDict("CommonTools.TurnOn", "CommonTools", [] { return new CommonTools(); });
}
