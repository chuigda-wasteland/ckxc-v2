#ifndef SOURCERANGE_HPP
#define SOURCERANGE_HPP

#include <cstddef>
#include <cstdint>

#include "sona/either.hpp"
#include "sona/small_vector.hpp"

namespace ckx {

using Coord = std::uint16_t;

class SourceLocation {
public:
  SourceLocation(Coord line, Coord col)
      : m_Line(line), m_Col(col) {}
  Coord GetLine() const { return m_Line; }
  Coord GetCol() const { return m_Col; }

private:
  Coord m_Line, m_Col;
};

class SingleSourceRange {
public:
  SingleSourceRange(Coord startLine, Coord startCol, Coord endCol)
    : m_StartLine(startLine), m_StartCol(startCol), m_EndCol(endCol) {}


  Coord GetStartLine() const { return m_StartLine; }
  Coord GetStartCol() const { return m_StartCol; }
  Coord GetEndCol() const { return m_EndCol; }

private:
  Coord m_StartLine, m_StartCol, m_EndCol;
};

using SourceRange = SingleSourceRange;

} // namespace ckx

#endif // SOURCERANGE_HPP
