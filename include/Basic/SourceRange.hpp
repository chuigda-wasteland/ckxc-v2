#ifndef SOURCERANGE_HPP
#define SOURCERANGE_HPP

#include <cstddef>
#include <cstdint>

#include "sona/small_vector.hpp"
#include "sona/either.hpp"

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
public:
    SourceRange(SingleSourceRange const& rng) :
        m_SourceRange(rng) {}
    SourceRange(std::initializer_list<SingleSourceRange> rngs) :
        m_SourceRange(rngs) {}

private:
    sona::either<SingleSourceRange, MultipleSourceRange> m_SourceRange;
};

} // namespace ckx

#endif // SOURCERANGE_HPP
