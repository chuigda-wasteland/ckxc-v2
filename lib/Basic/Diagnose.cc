#include "Basic/Diagnose.h"
#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>

namespace ckx {
namespace Diag {

std::string
Format(DiagMessageTemplate messageTemplate,
       std::vector<sona::strhdl_t> const& paramStrings) {
  char const *templateStr = nullptr;
  switch (messageTemplate) {
  #define DIAG_TEMPLATE(ID, STR) \
  case DMT_##ID: templateStr = STR; break;
  #include "Basic/Diags.def"
  default:
    sona_unreachable1("unhandled diag kind");
  }

  auto iter = paramStrings.begin();
  std::stringstream sstream;
  for (size_t idx = 0; templateStr[idx] != '\0';) {
    if (templateStr[idx] == '{' && templateStr[idx+1] == '}') {
      idx += 2;
      if (iter == paramStrings.end()) {
        sona_unreachable1("not enough param string provided");
      }
      sstream << (*iter).get();
      ++iter;
    }
    else {
      sstream << templateStr[idx];
      ++idx;
    }
  }

  if (iter != paramStrings.end()) {
    sona_unreachable1("redundant param string provided");
  }

  std::string ret;
  std::getline(sstream, ret);
  return ret;
}

DiagnosticEngine::DiagnosticInfo&
DiagnosticEngine::Diag(DiagnosticInfoRank rank,
                       std::string &&message, SourceRange const& range) {
  m_PendingDiags.push_back(DiagnosticInfo(rank));
  DiagnosticInfo& theInfo = m_PendingDiags.back();
  theInfo.AddDesc(std::move(message), range);
  return theInfo;
}

bool DiagnosticEngine::HasPendingError() const noexcept {
  for (DiagnosticInfo const& info : m_PendingDiags) {
    if (info.m_Rank == DiagnosticInfoRank::DIR_Error) {
      return true;
    }
  }
  return false;
}

bool DiagnosticEngine::HasPendingDiags() const noexcept {
  return m_PendingDiags.size() != 0;
}

void DiagnosticEngine::EmitDiags() {
  for (DiagnosticInfo const& info : m_PendingDiags) {
    info.Dump(m_FileName, m_CodeLines);
  }
  ClearDiags();
}

void DiagnosticEngine::ClearDiags() noexcept {
  m_PendingDiags.clear();
}

DiagnosticEngine::DiagnosticInfo&
DiagnosticEngine::DiagnosticInfo::AddDesc(std::string &&message,
                                          SourceRange const& range) {
  AddSubDiagnose(SDK_Desc, std::move(message), range);
  return *this;
}

DiagnosticEngine::DiagnosticInfo&
DiagnosticEngine::DiagnosticInfo::AddNote(std::string &&message,
                                          SourceRange const& range) {
  AddSubDiagnose(SDK_Note, std::move(message), range);
  return *this;
}

DiagnosticEngine::DiagnosticInfo&
DiagnosticEngine::DiagnosticInfo::AddFixit(std::string &&message,
                                           SourceRange const& range) {
  AddSubDiagnose(SDK_Fixit, std::move(message), range);
  return *this;
}

void DiagnosticEngine::DiagnosticInfo::AddSubDiagnose(
    SubDiagnoseKind sdk, std::string&& desc, SourceRange const& range) {
  m_SDKs.push_back(sdk);
  m_Messages.push_back(std::move(desc));
  m_SourceRanges.push_back(range);
}

static void PrintSourceCode(const std::vector<std::string>& codeLines,
                            SourceRange const& range) {
  using std::cerr;
  using std::endl;
  using std::setw;
  using std::setfill;

  /// the range was not implemented
  if (range.GetStartCol() == 0 && range.GetEndCol() == 0
      && range.GetStartLine() == 0) {
    cerr << "  <not-available> | Sorry, source info not implemented yet."
         << endl << endl;
    return;
  }

  sona_assert(codeLines.size() >= range.GetStartLine() - 1);
  cerr << " " << setw(5) << setfill('0') << range.GetStartLine()
       << " | " << codeLines[range.GetStartLine() - 1] << endl;

  for (size_t i = 0; i < 9; i++) {
    cerr.put(' ');
  }

  for (size_t i = 0; i < range.GetStartCol()-1; i++) {
    cerr.put(' ');
  }
  for (size_t i = range.GetStartCol()-1; i < range.GetEndCol()-1; i++) {
    cerr.put('^');
  }

  cerr << endl;
}

void
DiagnosticEngine::DiagnosticInfo::Dump(
    const std::string& fileName,
    const std::vector<std::string>& codeLines) const noexcept {
  using std::cerr;
  using std::endl;

  cerr << fileName << ':'
       << "(" << m_SourceRanges.front().GetStartLine() << ','
       << m_SourceRanges.front().GetStartCol() << "): ";

  switch (m_Rank) {
  case DIR_Error:
    cerr << "error: "; break;
  case DIR_Warning0: case DIR_Warning1: case DIR_Warning2:
    cerr << "warning: "; break;
  case DIR_Note:
    cerr << "note: "; break;
  }

  cerr << m_Messages.front();
  cerr << endl;

  PrintSourceCode(codeLines, m_SourceRanges.front());

  for (size_t i = 1; i < m_SDKs.size(); i++) {
    cerr << fileName << ':'
         << "(" << m_SourceRanges[i].GetStartLine() << ','
         << m_SourceRanges[i].GetStartCol() << "): ";

    switch (m_SDKs[i]) {
    case SDK_Desc:
      switch (m_Rank) {
      case DIR_Error:
        cerr << "error: "; break;
      case DIR_Warning0: case DIR_Warning1: case DIR_Warning2:
        cerr << "warning: "; break;
      case DIR_Note:
        cerr << "note: "; break;
      }
      break;
    case SDK_Note: cerr << "note: "; break;
    case SDK_Fixit: cerr << "fix hint: "; break;
    }

    cerr << m_Messages[i] << endl;
    PrintSourceCode(codeLines, m_SourceRanges[i]);
  }
}

} // namespace Diag
} // namespace ckx
