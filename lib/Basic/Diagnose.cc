#include "Basic/Diagnose.h"
#include <sstream>

namespace ckx {
namespace Diag {

std::string
FormatDiagMessage(DiagMessageTemplate messageTemplate,
                  std::vector<sona::string_ref> const& paramStrings) {
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
  m_PendingDiags.emplace_back(rank);
  DiagnosticInfo& theInfo = m_PendingDiags.back();
  theInfo.AddDesc(std::move(message), range);
  return theInfo;
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

void DiagnosticEngine::DiagnosticInfo::Dump() const noexcept {
  /// @todo
}

} // namespace Diag
} // namespace ckx
