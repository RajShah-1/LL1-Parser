#ifndef __LL1_PARSER_HPP__
#define __LL1_PARSER_HPP__

#include <cassert>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/constants.hpp"
#include "../Common/structs.hpp"

using namespace std;

class LL1Parser {
  int totNumSyms;
  Symbol* startSymbol;
  Symbol* epsSymbol;
  Symbol* dollarSymbol;
  vector<Symbol*> terminals;
  vector<Symbol*> nonTerminals;

  // map symbol-strings to corresponding symbol-pointer
  unordered_map<string, Symbol*> symToPtr;
  // map (symbol-ptr) to (production rules with that symbol on lhs)
  unordered_map<Symbol*, unordered_set<ProductionRule*>> productionRules;

  // Parsing table:
  //
  //    map (current non-terminal ->
  //          [
  //              map (current input symbol ->
  //                [
  //                    production rule to be used for the pair
  //                    {current non-terminal, current-input-symbol}
  //                ]
  //              )
  //          ]
  //        )
  unordered_map<Symbol*, unordered_map<Symbol*, ProductionRule*>> parsingTable;

  // Relative path of grammar's folder from current directory
  string dirPath;

 public:
  LL1Parser(const string& path);
  bool predictiveParsing(const vector<string>& tokens) const;
  void printCFG() const;
  void setDirPath(const string& dirPath);
  void readLL1File(istream& in);
  void readParseTableFile(istream& in);
};

// Operator-overloadings for easy printing
// print stack
ostream& operator<<(ostream& os, stack<Symbol*> st);
// print vector from pair.second.first to pair.second.second
// (pair.second.first inclusive and pair.second.second exclusive)
ostream& operator<<(ostream& os,
                    const pair<vector<string>, pair<int, int>>& vec);
#endif