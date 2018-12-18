#include "Basic/Diagnose.h"
#include <iostream>

int main() {
  using namespace std;
  using namespace sona;
  using namespace ckx;

  vector<string> codeLines {
    "int add(int a, int b) { return a + b; }",
    "int add(int a, int b) { return a - b; }"
  };

  Diag::DiagnosticEngine engine("main.cpp", codeLines);

  engine.Diag(Diag::DIR_Error,
              Diag::Format(Diag::DMT_Example, {"add"}),
              SourceRange(2, 5, 8)).
      AddNote(Diag::Format(Diag::DMT_Example2, {"add"}),
              SourceRange(1, 5, 8));
  engine.EmitDiags();

  return 0;
}
