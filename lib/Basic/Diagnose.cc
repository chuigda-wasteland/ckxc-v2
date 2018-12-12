#include "Basic/Diagnose.h"

using namespace ckx::Diag;

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
