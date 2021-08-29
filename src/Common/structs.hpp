#ifndef __COMMON_STRUCTS_HPP__
#define __COMMON_STRUCTS_HPP__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Symbol {
  int id;
  string symbol;
  bool isTerminal;
  Symbol(int id, string symbol, bool isTerminal) {
    this->id = id;
    this->symbol = symbol;
    this->isTerminal = isTerminal;
  }
  // Operator-overloading for easy output (must be supplied externally)
  friend ostream& operator<<(ostream& os, const Symbol* sym);
};

struct ProductionRule {
  Symbol* lhs;
  vector<Symbol*> rhs;
  ProductionRule(Symbol* lhs, vector<Symbol*>& rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
  }
  // Operator-overloading for easy output (must be supplied externally)
  friend ostream& operator<<(ostream& os, const ProductionRule* sym);
};

#endif