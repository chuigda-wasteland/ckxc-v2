#include "Basic/Diagnose.h"
#include <iostream>

int main() {
  using namespace std;
  using namespace sona;
  using namespace ckx;

  vector<string> codeLines {
    "this is a fire starting in my heart",
    "that was an fire starting in my heart"
  };

  Diag::DiagnosticEngine engine("lyric", codeLines);
  engine.Diag(Diag::DiagnosticEngine::DIR_Error,
              Diag::FormatDiagMessage(Diag::DMT_Example, {"was", "is"}),
              SourceRange(2, 6, 9)).
      AddNote(Diag::FormatDiagMessage(Diag::DMT_Example2, {"is"}),
              SourceRange(1, 6, 8));
  engine.EmitDiags();

  return 0;
}
