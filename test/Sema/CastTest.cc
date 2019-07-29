#include "VKTestCXX.h"
#include "Sema/SemaPhase1.h"

using namespace sona;
using namespace ckx;
using namespace std;

class SemaPhase1Test : public Sema::SemaPhase1 {
public:
  using SemaPhase1::TryImplicitCast;
  using SemaPhase1::ActOnStaticCast;
  using SemaPhase1::ActOnConstCast;
};
