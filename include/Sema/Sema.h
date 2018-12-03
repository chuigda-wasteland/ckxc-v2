#ifndef SEMA_H
#define SEMA_H

#include "Syntax/CSTFwd.h"
#include "AST/DeclFwd.hpp"

#include "sona/pointer_plus.hpp"

namespace ckx {
namespace Sema {

class Sema final {
public:
  sona::owner<AST::TransUnitDecl>
  ActOnTransUnit(sona::ref_ptr<Syntax::TransUnit> transUnit);
};

} // namespace Sema
} // namespace ckx

#endif // CSTANNOTATOR_H
