#ifndef __LL1_HPP__
#define __LL1_HPP__

#include <cassert>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

const string EPSILON_SYMBOL = "ε";
const string DOLLAR_SYMBOL = "$";
const string NOT_LL1_EXCEPTION = "The CFG is not an LL(1) grammar";

struct Symbol {
  int id;
  string symbol;
  bool isTerminal;
  Symbol(int id, string symbol, bool isTerminal) {
    this->id = id;
    this->symbol = symbol;
    this->isTerminal = isTerminal;
  }
  // Operator-overloading for serialization
  friend ostream& operator<<(ostream& os, const Symbol* sym);
};

struct ProductionRule {
  Symbol* lhs;
  vector<Symbol*> rhs;
  ProductionRule(Symbol* lhs, vector<Symbol*>& rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
  }
  // Operator-overloading for serialization
  friend ostream& operator<<(ostream& os, const ProductionRule* sym);
};

class LL1 {
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

  // First and Follow sets
  unordered_map<Symbol*, unordered_set<Symbol*>> firstSetsMap;
  unordered_map<Symbol*, unordered_set<Symbol*>> followSetsMap;

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

  void computeFirstForSym(Symbol* sym);
  void printProductionRule(const ProductionRule* pr);

 public:
  LL1();
  void eliminateLeftRecursion();
  void leftFactor();
  void computeFirst();
  void computeFollow();
  bool isLL1();
  void buildParsingTable();
  bool predictiveParsing(const vector<string>& tokens) const;
  void readCFG();
  void printCFG();

  // Operator-overloading for serialization
};

// Operator-overloadings for easy printing
// print stack
ostream& operator<<(ostream& os, stack<Symbol*> st);
// print vector from pair.second.first to pair.second.second
// (pair.second.first inclusive and pair.second.second exclusive)
ostream& operator<<(ostream& os,
                    const pair<vector<string>, pair<int, int>>& vec);
#endif