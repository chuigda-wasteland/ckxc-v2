#ifndef DIAGNOSE_H
#define DIAGNOSE_H

#include "Basic/SourceRange.hpp"

#include <vector>
#include <string>
#include "sona/stringref.hpp"

namespace ckx {
namespace Diag {

enum class DiagMessageTemplate {
  /// @todo use table generation to generate this
};

std::string
FormatDiagMessage(DiagMessageTemplate messageTemplate,
                  std::initializer_list<sona::string_ref> paramStrings);

class DiagnosticEngine {
private:
  class DiagnosticInfo;

public:
  enum DiagnosticInfoRank {
    DIR_Error, DIR_Warning0, DIR_Warning1, DIR_Warning2, DIR_Note
  };

  DiagnosticInfo& Diag(DiagnosticInfoRank rank,
                       std::string &&message,
                       SourceRange const& range);

  bool HasError() const noexcept;
  bool HasDiagInfo() const noexcept;
  void ClearDiags() noexcept;

private:
  class DiagnosticInfo {
    friend class DiagnosticEngine;

  public:
    enum SubDiagnoseKind { SDK_Desc, SDK_Note, SDK_Fixit };

    DiagnosticInfo(DiagnosticInfoRank rank) : m_Rank(rank) {}

    DiagnosticInfo& AddDesc(std::string &&desc,
                            SourceRange const& range);

    DiagnosticInfo& AddNote(std::string &&desc,
                            SourceRange const& range);

    DiagnosticInfo& AddFixit(std::string &&desc,
                             SourceRange const& range);

  private:
    void AddSubDiagnose(SubDiagnoseKind sdk, std::string &&desc,
                        SourceRange const& range);

    void Dump() const noexcept;

    DiagnosticInfoRank m_Rank;
    std::vector<SubDiagnoseKind> m_SDKs;
    std::vector<std::string> m_Messages;
    std::vector<SourceRange> m_SourceRanges;
  };

  std::vector<DiagnosticInfo> m_PendingDiags;
};

} // namespace Diag
} // namespace ckx

#endif // DIAGNOSE_H
