#ifndef _COMMONTOOLS_H_
#define _COMMONTOOLS_H_

#include <string>
#include <vector>
#include "OffSM4topsTool/TruthHelpers.h"
#include "OffSM4topsTool/json.hpp"
#include "TTHbbObjects/Event.h"
#include "TTHbbToolManager/ToolBase.h"

namespace TTHbb {

class CommonTools : public ToolBase {
  /**
   * @brief Tool for postproduction of SSML mini-ntuples
   * Compute additionnal global variables (e.g. dRlj etc...
   */

 public:
  /// Constructor
  CommonTools(){};

  /// Destructor
  ~CommonTools(){};

  /// Function to initialise objects
  void initialise();

  /// Fill new variables
  void apply(TTHbb::Event* event);

  /// Function to finalise
  void finalise();

 private:
  SM4t::TruthHelpers* m_truthHelper = nullptr;
  bool isData;
  void AddTtbarCombinationWeight(TTHbb::Event* event);
  float WeightTtbarInclHTSlicedHFFiltered(int dsid, float truth_ht, int flavor_filter_flag);

  std::string m_ttbarStitchingDsidMapPath;
  nlohmann::json m_ttbarStitchingDsidMap;
};
}  // namespace TTHbb
#endif
