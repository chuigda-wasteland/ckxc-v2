#include "Syntax/CST.h"
#include <algorithm>

using namespace ckx;
using namespace Syntax;
using namespace std;
using namespace sona;

namespace ckx {
namespace Syntax {

BasicType::TypeKind LiteralExpr::EvaluateIntTypeKind(int64_t i) noexcept {
  if (i <= std::numeric_limits<int8_t>::max()
      && i >= std::numeric_limits<int8_t>::min()) {
    return BasicType::TypeKind::TK_Int8;
  }
  else if (i < std::numeric_limits<int16_t>::max()
           && i >= std::numeric_limits<int16_t>::min()) {
    return BasicType::TypeKind::TK_Int16;
  }
  else if (i < std::numeric_limits<int32_t>::max()
           && i >= std::numeric_limits<int32_t>::min()) {
    return BasicType::TypeKind::TK_Int32;
  }
  else {
    return BasicType::TypeKind::TK_Int64;
  }
}

BasicType::TypeKind LiteralExpr::EvaluateUIntTypeKind(uint64_t u) noexcept {
  if (u <= std::numeric_limits<uint8_t>::max()
      && u >= std::numeric_limits<uint8_t>::min()) {
    return BasicType::TypeKind::TK_UInt8;
  }
  else if (u < std::numeric_limits<uint16_t>::max()
           && u >= std::numeric_limits<uint16_t>::min()) {
    return BasicType::TypeKind::TK_UInt16;
  }
  else if (u < std::numeric_limits<uint32_t>::max()
           && u >= std::numeric_limits<uint32_t>::min()) {
    return BasicType::TypeKind::TK_UInt32;
  }
  else {
    return BasicType::TypeKind::TK_UInt64;
  }
}

BasicType::TypeKind LiteralExpr::EvaluateFloatTypeKind(double) noexcept {
  return BasicType::TypeKind::TK_Double;
}

} // namespace Syntax
} // namespace ckx
