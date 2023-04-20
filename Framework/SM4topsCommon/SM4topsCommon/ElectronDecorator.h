
#ifndef ElectronDecorator_H_
#define ElectronDecorator_H_

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEventSelectionTools/EventSelectorBase.h"

#include <AsgTools/MessageCheck.h>

using namespace asg::msgUserCode;

namespace SM4topsCommon {

class ElectronDecorator : public top::EventSelectorBase {
 public:
  ElectronDecorator(const std::string &selectorName, const std::string &params,
                    std::shared_ptr<top::TopConfig> config);
  ~ElectronDecorator() = default;
  // void initialise(const top::Event& event) const;
  virtual bool apply(const top::Event &event) const override;
  std::shared_ptr<top::TopConfig> m_config;

  std::string name() const override;

 private:
  std::vector<std::string> m_electronTriggerNames;
  std::vector<std::string> m_electronSLTriggerNames;
  std::vector<std::string> m_electronDLTriggerNames;
  bool m_isMC;
};
}  // namespace SM4topsCommon
#endif
