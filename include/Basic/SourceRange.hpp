#ifndef SOURCERANGE_HPP
#define SOURCERANGE_HPP

#include <cstddef>
#include <cstdint>

#include "sona/small_vector.hpp"

namespace ckx {

using Coord = std::uint16_t;

class SingleSourceRange {
public:
    SingleSourceRange(Coord startLine, Coord startCol, Coord endCol) :
        m_StartLine(startLine), m_StartCol(startCol), m_EndCol(endCol) {}

    Coord GetStartLine() const { return m_StartLine; }
    Coord GetStartCol() const { return m_StartCol; }
    Coord GetEndCol() const { return m_EndCol; }

private:
    Coord m_StartLine, m_StartCol, m_EndCol;
};

using MultipleSourceRange = sona::small_vector<SingleSourceRange, 2>;

class SourceRange {
private:
    /// sona::either<SingleSourceRange, MultipleSourceRange> e;
    /// @todo finish sona::either then.
};

} // namespace ckx

#endif // SOURCERANGE_HPP
