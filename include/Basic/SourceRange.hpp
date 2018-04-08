#ifndef SOURCERANGE_HPP
#define SOURCERANGE_HPP

#include <cstddef>
#include <cstdint>

#include "sona/small_vector.hpp"
#include "sona/either.hpp"

namespace ckx {

using Coord = std::uint16_t;
using SourceFileId = std::uint16_t;

class SourceLocation {
public:
    SourceLocation(SourceFileId fileId, Coord line, Coord col)
        : m_FileId(fileId), m_Line(line), m_Col(col) {}
    SourceFileId GetFileId() const { return m_FileId; }
    Coord GetLine() const { return m_Line; }
    Coord GetCol() const { return m_Col; }

private:
    SourceFileId m_FileId;
    Coord m_Line, m_Col;
};

class SingleSourceRange {
public:
    SingleSourceRange(SourceFileId fileId, Coord startLine, Coord startCol,
                      Coord endCol) :
        m_FileId(fileId), m_StartLine(startLine), m_StartCol(startCol),
        m_EndCol(endCol) {}

    SourceFileId GetFileId() const { return m_FileId; }
    Coord GetStartLine() const { return m_StartLine; }
    Coord GetStartCol() const { return m_StartCol; }
    Coord GetEndCol() const { return m_EndCol; }

private:
    SourceFileId m_FileId;
    Coord m_StartLine, m_StartCol, m_EndCol;
};

using SourceRange = sona::small_vector<SingleSourceRange, 2>;

} // namespace ckx

#endif // SOURCERANGE_HPP
