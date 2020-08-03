#include "Frontend/Lex.h"
#include "Frontend/Parser.h"
#include "Sema/SemaPhase0.h"
#include "Sema/SemaPhase1.h"
#include "Backend/ReplInterpreter.h"

#include "sona/strutil.h"
#include "sona/egconf.h"
#include <fstream>
#include <iostream>

#include <clocale>
#include <uchar.h>
#include <unistd.h>

using namespace sona;
using namespace ckx;
using namespace std;

class SemaPhase0ForRepl : public Sema::SemaPhase0 {
public:
  SemaPhase0ForRepl(AST::ASTContext &astContext,
                    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                    Diag::DiagnosticEngine &diag)
    : SemaPhase0(astContext, declContexts, diag) {}
  using SemaPhase0::ActOnVarDecl;
};

class SemaPhase1ForRepl : public Sema::SemaPhase1 {
public:
  SemaPhase1ForRepl(AST::ASTContext &astContext,
                    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                    Diag::DiagnosticEngine &diag)
    : SemaPhase1(astContext, declContexts, diag) {}
  using SemaPhase1::ActOnExpr;
  using SemaPhase1::GetCurrentScope;
};

void HiddenPresent() {
  const string graph =
      " .*.        /~ .~\\    /~  ~\\    /~ .~\\    /~  ~\\\n"
      " ***       '      `\\/'      *  '      `\\/'      *\n"
      "  V       (                .*)(               . *)\n"
      "/\\|/\\      \\      CTZ   . *./  \\      CKX   . *./\n"
      "  |         `\\ .      . .*/'    `\\ .      . .*/'       .*.\n"
      "  |           `\\ * .*. */' _    _ `\\ * .*. */'         ***\n"
      "                `\\ * */'  ( `\\/'*)  `\\ * */'          /\\V\n"
      "                  `\\/'     \\   */'    `\\/'              |/\\\n"
      "                            `\\/'                        |\n"
      "                            LOVES\n"
      "\n";

  auto msleep = [] (unsigned ms) { usleep(ms * 1000); };

  cerr << "警告：检测到内部异常" << endl;
  cerr << "正在运行自我检测..." << endl;
  msleep(1250);
  for (int i = 0; i < 100;) {
    cerr << "自检中：" << i << "%，请勿退出程序" << endl;
    msleep(250);
    i += rand() % 10;
    if (i > 100) {
      i = 100;
    }
  }

  cerr << "自检完毕，正在输出异常信息..." << endl << endl;
  msleep(1250);

  for (char ch : graph) {
    cerr << ch;
    msleep(25);
  }

  std::setlocale(LC_ALL, "en_US.utf8");
  const u32string text =
      U"\t拼凑着残破的竹简\n"
      "\t誊录下看不懂的字段\n"
      "\t晦涩古老的字句之间\n"
      "\t流淌着悠远的思念\n"
      "\t思念着千年前那一场雨\n"
      "\t乘着风洋洋洒洒飞向大地\n"
      "\t全世界预见了这一季花期\n"
      "\t而我在海棠树下遇见了你\n"
      "\t看春风 和着雨 浩如烟\n"
      "\t看繁花 烟雨中 更绚烂\n"
      "\t我执笔 在树下 记录着从前\n"
      "\t你却问 我可是 海棠仙\n"
      "\t小巷里 让细雨 穿成线\n"
      "\t浸透在 海棠色 的华年\n"
      "\t你和我 漫步过 滴水的屋檐\n"
      "\t描绘着 不存在 的永远\n"
      "\t穿越千年 草长飞鸢 与你手儿牵\n"
      "\t隔着时空 为我写下 最美的画卷\n"
      "\t看着那阳光下海棠的雨\n"
      "\t乘起风洋洋洒洒翱翔天际\n"
      "\t就像谁都留不住片刻花期\n"
      "\t你我也逃不掉最后的别离\n"
      "\t看春风 和着雨 浩如烟\n"
      "\t看繁花 烟雨中 被吹散\n"
      "\t我执伞 在树下 合上了书卷\n"
      "\t别过了 海棠花 开渐渐\n"
      "\t小巷里 让细雨 穿成线\n"
      "\t浸透在 海棠色 的华年\n"
      "\t古巷里 我抚过 锈绿的铜环\n"
      "\t回忆着 你说过 的永——\n"
      "\t看春风 和着雨 浩如烟\n"
      "\t看繁花 烟雨中 更绚烂\n"
      "\t我抬手 在树下 拼凑着竹简\n"
      "\t摩挲过 你等待 的千年\n"
      "\t心中是 爱你的 海棠仙\n";
  for (char32_t ch : text) {
    if (ch == '\t') {
      msleep(500);
    }
    else {
      char buffer[8];
      std::mbstate_t state{};
      size_t loc = c32rtomb(buffer, ch, &state);
      buffer[loc] = '\0';
      cerr << buffer;
      msleep(125);
    }
  }
}

int main() {
  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  Backend::ReplInterpreter replInterp;

  for(;;) {
    vector<string> lines;
    string line;

    cerr << "[ckxc-v2-repl *Basic] λ ";
    getline(cin, line);
    if (line == "" || line == "quit" || line == "exit") {
      cerr << endl << "  Moriturus te saluto." << endl;
      return 0;
    }

    lines.push_back(line);

    Diag::DiagnosticEngine diag("<repl-input>", lines);
    Frontend::Lexer lexer(std::move(line), diag);
    if (diag.HasPendingDiags()) {
      if (diag.HasPendingError()) {
        diag.EmitDiags();
        return -1;
      }
      diag.EmitDiags();
    }

    vector<Frontend::Token> tokens = lexer.GetAndReset();
    Frontend::Parser parser(diag);
    SemaPhase0ForRepl sp0(astContext, declContexts, diag);
    SemaPhase1ForRepl sp1(astContext, declContexts, diag);

    if (tokens.front().GetTokenKind() == Frontend::Token::TK_KW_def) {
      owner<Syntax::VarDecl> decl = parser.ParseVarDecl(tokens);
      // owner<AST::VarDecl> decl1 =
      //    sp0.ActOnVarDecl(decl.borrow()).first.cast_unsafe<AST::VarDecl>();
      // replInterp.DefineVar(decl1.borrow());
      if (diag.HasPendingDiags()) {
        diag.EmitDiags();
        continue;
      }
      cerr << "  Sorry, variable decalrations are not supported yet." << endl;
    }
    else {
      owner<Syntax::Expr> expr = parser.ParseExpr(tokens);
      if (diag.HasPendingDiags()) {
        diag.EmitDiags();
        continue;
      }
      owner<AST::Expr> expr1 = sp1.ActOnExpr(sp1.GetCurrentScope(),
                                             expr.borrow());
      if (diag.HasPendingDiags()) {
        diag.EmitDiags();
        continue;
      }
      Backend::ReplValue value = expr1.borrow()->Accept(replInterp).borrow()
                                               ->GetValue<Backend::ReplValue>();

      sona::ref_ptr<AST::BuiltinType const> ty =
          expr1.borrow()->GetExprType()
               .GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
      if (ty->IsSigned()) {
        cerr << "  Result: " << value.GetIntValue() << endl << endl;
        if (value.GetIntValue() == 520) {
          HiddenPresent();
          return 0;
        }
      }
      else if (ty->IsUnsigned()) {
        cerr << "  Result: " << value.GetUIntValue() << endl << endl;
      }
      else if (ty->IsFloating()) {
        cerr << "  Result: " << value.GetFloatValue() << endl << endl;
      }
      else /* if (ty->IsBool()) */ {
        cerr << "  Result: " << boolalpha
             << value.GetBoolValue() << endl << endl;
      }
    }
  }
}
