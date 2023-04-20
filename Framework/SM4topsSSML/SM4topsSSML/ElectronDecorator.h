
#ifndef ElectronDecorator_H_
#define ElectronDecorator_H_

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEventSelectionTools/EventSelectorBase.h"

#include <AsgTools/MessageCheck.h>
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"

using namespace asg::msgUserCode;

namespace SM4topsSSML {

class ElectronDecorator : public top::EventSelectorBase {
 public:
  ElectronDecorator(const std::string &params, std::shared_ptr<top::TopConfig> config);
  ~ElectronDecorator() = default;

  // void initialise(const top::Event& event) const;
  virtual bool apply(const top::Event &event) const override;

  std::string name() const override;
  TTHbbAsgToolHelper* m_asgHelper;

 private:

  InDet::InDetTrackSelectionTool m_trkseltool;
  bool m_isRemote;
};
}  // namespace SM4topsSSML
#endif
