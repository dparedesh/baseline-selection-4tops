#ifndef _TRUTHHELPERS_H_
#define _TRUTHHELPERS_H_

#include <bitset>  // std::bitset. Use for readability
#include "TTHbbObjects/Lepton.h"
namespace SM4t {

class TruthHelpers {
 public:
  enum SM4tTruthCategoryFlag {
    Other = 0,
    Prompt = 1,
    PromptFlipQ = 2,
    Trident = 3,
    TridentFlipQ = 4,
    MaterialConv = 5,
    GamstarConv = 6,
    HFc = 7,
    HFb = 8,
    Fake = 9,
    TRUTHCATFLAGMAX = 10
  };

  enum SM4tBkgCategory {
    UNDEFINED=-1,
    prompt=0,
    qmID=1,
    materialconv=2,
    gamstarconv=3,
    HF=4,
    fake=5,
    other=6,
    BKGCATMAX=7

  };

  TruthHelpers() = default;
  ~TruthHelpers() = default;

  bool isFromPromptSource(int origin);
  bool isFromSMPromptSource(int origin);
  bool isFromLightSource(int origin);
  bool hasBHadronOrigin(int origin);
  bool hasCHadronOrigin(int origin);
  bool isPromptElectron(int type, int origin, int motherPdgid, int motherType, int motherOrigin);
  bool isChargeFlip(int charge, int motherPdgid);

  std::bitset<7> leptonCategorisationIFF(std::shared_ptr<TTHbb::Lepton> lep);

  int electronCategorisationZhi(std::shared_ptr<TTHbb::Lepton> el);
  int muonCategorisationZhi(std::shared_ptr<TTHbb::Lepton> mu);
  int leptonCategorisationZhi(std::shared_ptr<TTHbb::Lepton> lep);

  SM4tTruthCategoryFlag electronBkgFlagSM4t(std::shared_ptr<TTHbb::Lepton> lep);
  SM4tTruthCategoryFlag muonBkgFlagSM4t(std::shared_ptr<TTHbb::Lepton> lep);
  SM4tTruthCategoryFlag leptonBkgFlagSM4t(std::shared_ptr<TTHbb::Lepton> lep);

  SM4tBkgCategory leptonBkgCategorisation(int chan, SM4tTruthCategoryFlag lep0, SM4tTruthCategoryFlag lep1, SM4tTruthCategoryFlag lep2);

 private:
};
}  // namespace SM4t

#endif
