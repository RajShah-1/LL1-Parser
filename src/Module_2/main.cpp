#include "LL1Parser.hpp"
#include "lexFns.hpp"

int main() {
  string dirPath;
  cout << "Enter a (relative/absolute) path of a directory where the grammar's "
          "files should be saved:\n";
  cin >> dirPath;

  LL1Parser ll1(dirPath);
  vector<string> tokens;

  cout << "Test-input:\n";
  int token = yylex();
  while (token) {
    if (token == -1) {
      cout << "Error in line " << line_number << ", Rejecting: " << yytext
           << "\n";
    } else {
      cout << "Token: " << yy_token_type << "\n";
      tokens.push_back(yy_token_type);
    }
    token = yylex();
  }
  tokens.push_back(DOLLAR_SYMBOL);

  bool isValid = ll1.predictiveParsing(tokens);
  cout << "Verdict: " << (isValid ? "Accepted" : "Rejected") << "\n";
  return 0;
}
