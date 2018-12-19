#include "Frontend/Lex.h"
#include "Frontend/Parser.h"
#include "sona/strutil.h"
#include <fstream>
#include <iostream>

using namespace sona;
using namespace ckx;
using namespace std;

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    cerr << "usage ckx-syntax filename" << endl;
    return -1;
  }

  std::ifstream ifs (argv[1]);
  if (!ifs.is_open()) {
    cerr << "unable to open file" << endl;
    return -1;
  }

  vector<string> lines;
  string wholeSource;
  while (!ifs.eof()) {
    string line;
    getline(ifs, line);
    wholeSource += (line + '\n');
    lines.push_back(std::move(line));
  }

  Diag::DiagnosticEngine diag(argv[1], lines);
  Frontend::Lexer lexer(std::move(wholeSource), diag);
  if (diag.HasPendingDiags()) {
    if (diag.HasPendingError()) {
      diag.EmitDiags();
      return -1;
    }
    diag.EmitDiags();
  }

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  Frontend::Parser parser(diag);
  owner<Syntax::TransUnit> unit = parser.ParseTransUnit(tokens);

  if (diag.HasPendingDiags()) {
    if (diag.HasPendingError()) {
      diag.EmitDiags();
      return -1;
    }
    diag.EmitDiags();
  }

  cerr << "Parsing success, no syntactical issue" << endl;

  return 0;
}
