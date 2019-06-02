#ifndef DIAGNOSE_H
#define DIAGNOSE_H

#include "Basic/SourceRange.h"

#include <vector>
#include <string>
#include <initializer_list>
#include "sona/stringref.h"

namespace ckx {
namespace Diag {

enum DiagMessageTemplate {
#define DIAG_TEMPLATE(ID, TMPSTR) DMT_##ID ,
#include "Basic/Diags.def"
  DMT_End
};

enum DiagnosticInfoRank {
  DIR_Error, DIR_Warning0, DIR_Warning1, DIR_Warning2, DIR_Note
};

std::string
Format(DiagMessageTemplate messageTemplate,
       const std::vector<sona::string_ref>& paramStrings);

class DiagnosticEngine {
private:
  class DiagnosticInfo;

public:
  DiagnosticEngine(std::string const& fileName,
                   std::vector<std::string> const& codeLines)
    : m_FileName(fileName), m_CodeLines(codeLines) {}

  DiagnosticInfo& Diag(DiagnosticInfoRank rank,
                       std::string &&message,
                       SourceRange const& range);

  bool HasPendingError() const noexcept;
  bool HasPendingDiags() const noexcept;
  void EmitDiags();
  void ClearDiags() noexcept;

private:
  class DiagnosticInfo {
    friend class DiagnosticEngine;

  public:
    enum SubDiagnoseKind { SDK_Desc, SDK_Note, SDK_Fixit };

    DiagnosticInfo& AddDesc(std::string &&desc,
                            SourceRange const& range);

    DiagnosticInfo& AddNote(std::string &&desc,
                            SourceRange const& range);

    DiagnosticInfo& AddFixit(std::string &&desc,
                             SourceRange const& range);

  private:
    DiagnosticInfo(DiagnosticInfoRank rank) : m_Rank(rank) {}

    void AddSubDiagnose(SubDiagnoseKind sdk, std::string &&desc,
                        SourceRange const& range);

    void Dump(std::string const& fileName,
              std::vector<std::string> const& codeLines) const noexcept;

    DiagnosticInfoRank m_Rank;
    std::vector<SubDiagnoseKind> m_SDKs;
    std::vector<std::string> m_Messages;
    std::vector<SourceRange> m_SourceRanges;
  };

  std::string m_FileName;
  std::vector<std::string> const& m_CodeLines;
  std::vector<DiagnosticInfo> m_PendingDiags;
};

} // namespace Diag
} // namespace ckx

#endif // DIAGNOSE_H
