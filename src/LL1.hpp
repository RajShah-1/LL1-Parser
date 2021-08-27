#ifndef __LL1_HPP__
#define __LL1_HPP__

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

const string EPSILON_SYMBOL = "ε";
const string DOLLAR_SYMBOL = "$";
const string NOT_LL1_EXCEPTION = "The CFG is not an LL(1) grammar";

class LL1 {
  struct Symbol {
    int id;
    string symbol;
    bool isTerminal;
    Symbol(int id, string symbol, bool isTerminal) {
      this->id = id;
      this->symbol = symbol;
      this->isTerminal = isTerminal;
    }
  };

  struct ProductionRule {
    Symbol* lhs;
    vector<Symbol*> rhs;
    ProductionRule(Symbol* lhs, vector<Symbol*>& rhs) {
      this->lhs = lhs;
      this->rhs = rhs;
    }
  };

  int totNumSyms;
  Symbol* startSymbol;
  Symbol* epsSymbol;
  Symbol* dollarSymbol;
  vector<Symbol*> terminals;
  vector<Symbol*> nonTerminals;
  // map (symbol-ptr) to (production rules with that symbol on lhs)
  unordered_map<Symbol*, unordered_set<ProductionRule*>> productionRules;

  // First and Follow sets
  unordered_map<Symbol*, unordered_set<Symbol*>> firstSetsMap;
  unordered_map<Symbol*, unordered_set<Symbol*>> followSetsMap;

  // Parsing table
  // map(current-symbol ->
  //  (
  //    map (current input symbol ->
  //          [map (input character) ->
  //                (production rule to be used for the pair
  //                {current-input-symbol, input-character})]
  // )
  unordered_map<Symbol*, unordered_map<Symbol*, ProductionRule*>> parsingTable;

  void computeFirstForSym(Symbol* sym);

 public:
  LL1();
  void eliminateLeftRecursion();
  void leftFactor();
  void computeFirst();
  void computeFollow();
  bool isLL1();
  void buildParsingTable();
  void predictiveParsing(const string& str);
  void readCFG();
  void printCFG();
};

#endif