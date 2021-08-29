#include "LL1.hpp"
#include "lexFns.hpp"

int main() {
  LL1 cfg;
  cfg.setDirPath("./grammar_1");
  cfg.createFirstFollowFile();
  cfg.createLL1GrammarFile();
  cfg.createParseTableFile();

  vector<string> tokens;
  // cout << "Test-input:\n";
  // cin.ignore(1000, '\n');
  // int token = yylex();
  // while (token) {
  //   if (token == -1) {
  //     cout << "Error in line " << line_number << ", Rejecting: " << yytext
  //          << "\n";
  //   } else {
  //     cout << "Token: " << yy_token_type << "\n";
  //     tokens.push_back(yy_token_type);
  //   }
  //   token = yylex();
  // }
  // tokens.push_back(DOLLAR_SYMBOL);
  // bool isValid = cfg.predictiveParsing(tokens);
  // cout << "Verdict: " << (isValid ? "Accepted" : "Rejected") << "\n";
  return 0;
}
