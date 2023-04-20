#ifndef LeptonOrdering_H_
#define LeptonOrdering_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopConfiguration/TopConfig.h"

namespace SM4topsSSML {

class LeptonOrdering : public top::EventSelectorBase
{
public:
  /**
   * @brief Order the lepton in decreasing pt.
   */
  explicit LeptonOrdering(const std::string& params, std::shared_ptr<top::TopConfig> config);

  /**
   * @brief Order the lepton in decreasing pt.
   *
   * @return True after ordering the leptons.
   */
  bool apply(const top::Event& event) const override;

  std::string name() const override;

private:

};

}

#endif
