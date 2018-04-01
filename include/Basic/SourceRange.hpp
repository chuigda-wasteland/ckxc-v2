#ifndef SOURCERANGE_HPP
#define SOURCERANGE_HPP

namespace ckx {

class SourceRange {
public:
    /// @note multiple-line source-range will be solved in other ways
    SourceRange(int startLine, int startCol, int endCol) :
        m_StartLine(startLine), m_StartCol(startCol), m_EndCol(endCol) {}

    int GetStartLine() const { return m_StartLine; }
    int GetStartCol() const { return m_StartCol; }
    int GetEndCol() const { return m_EndCol; }

private:
    int m_StartLine, m_StartCol, m_EndCol;
};

} // namespace ckx

#endif // SOURCERANGE_HPP
