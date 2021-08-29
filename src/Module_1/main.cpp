#include "LL1.hpp"
#include "lexFns.hpp"

int main() {
  LL1 cfg;
  string dirPath;
  cout << "Enter a (relative/absolute) path of a directory where the grammar's "
          "files should be saved:\n";
  cin >> dirPath;
  cfg.setDirPath(dirPath);
  cfg.createFirstFollowFile();
  cfg.createLL1GrammarFile();
  cfg.createParseTableFile();
  return 0;
}
