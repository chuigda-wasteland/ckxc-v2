#include "Basic/Diagnose.h"
#include <iostream>

int main() {
  using namespace std;
  using namespace sona;
  using namespace ckx;

  cout << Diag::FormatDiagMessage(Diag::DMT_Example, {"Class", "Klass"})
       << endl;

  return 0;
}
