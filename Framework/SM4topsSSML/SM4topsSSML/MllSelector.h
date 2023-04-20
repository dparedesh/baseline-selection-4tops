#ifndef MllSelector_H_
#define MllSelector_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace SM4topsSSML {
/**
 * @brief Put a cut on the invariant mass of two same flavour leptons.  If you
 * want to veto a window (say for the Z mass) you instead want MLLWindow.
 */
class MllSelector : public top::SignValueSelector 
{
 public:
  explicit MllSelector(const std::string& params, const bool& useLooseLeptons=false);

  bool apply(const top::Event& event) const override;
  std::string name() const override;

 private:
  bool m_useLooseLeptons;
};
}
#endif
