#include "LL1.hpp"

LL1::LL1() {
  this->totNumSyms = 0;
  // take cfg as an input from the user
  this->readCFG();
  // fills up: startSymbol, terminals, nonTerminals, and production-rules
  cout << "\n You entered:\n";
  this->printCFG();

  // Nested left recursion is removed, but nested left factors are not
  // seperated. So, if a grammar has nested left factoring, it will not be
  // successfully converted to an LL1 grammar.
  // Example of such grammar
  //    A -> B | abcd
  //    B -> abd
  //    (In this grammar, effectively A -> abd | abcd, this production-rule has
  //    a common factor "ab")
  // In this case, calling buildParseTable method might cause an exception
  this->eliminateLeftRecursion();
  this->leftFactor();

  cout << "\n After eliminating left-recursion and factoring the CFG:\n";
  this->printCFG();

  // propagate first and follow sets for all the symbols
  this->computeFirst();
  this->computeFollow();

  // build a parsing table
  // if the grammar is not LL(1), an exception is thrown by
  // buildParsingTable method
  this->buildParsingTable();
}

bool LL1::predictiveParsing(const vector<string>& tokens) const {
  int tokenIndex = 0;
  stack<Symbol*> st;
  st.push(this->dollarSymbol);
  st.push(this->startSymbol);

  while (!st.empty()) {
    cout << "Lookup: [" << st.top()->symbol << " " << tokens[tokenIndex]
         << "]\n";
    Symbol* stackTop = st.top();
    if (this->symToPtr.find(tokens[tokenIndex]) == this->symToPtr.end()) {
      cout << "Unexpected symbol: " << tokens[tokenIndex] << "\n";
      return false;
    }
    Symbol* tokenPtr = this->symToPtr.find(tokens[tokenIndex])->second;
    if (!tokenPtr->isTerminal || tokenPtr == this->epsSymbol) {
      cout << "Unexpected symbol: " << tokens[tokenIndex] << "\n";
      return false;
    }

    if (stackTop->isTerminal && stackTop->symbol == tokens[tokenIndex]) {
      st.pop();
      tokenIndex++;
      cout << "Match found: " << stackTop->symbol << "\nStack: " << st
           << "\nRemaining-I/p: "
           << make_pair(tokens, make_pair(tokenIndex, tokens.size())) << "\n";
      if (tokenIndex == tokens.size()) {
        return (st.size() == 0);
      }
    } else if (stackTop->isTerminal) {
      cout << "Expected: " << stackTop->symbol
           << " Found: " << tokens[tokenIndex] << "\n";
      return false;
    } else {
      if (this->parsingTable.find(stackTop) == this->parsingTable.end()) {
        cout << "No production rule can be applied!\n";
        return false;
      }
      auto symParseRow = this->parsingTable.find(stackTop)->second;
      if (symParseRow.find(tokenPtr) == symParseRow.end()) {
        cout << "No production rule can be applied!\n";
        return false;
      }
      ProductionRule* pr = symParseRow.find(tokenPtr)->second;
      st.pop();
      for (auto it = pr->rhs.rbegin(); it != pr->rhs.rend(); ++it) {
        if ((*it) == this->epsSymbol) continue;
        st.push(*it);
      }
      cout << "Applying production rule: " << pr << "\nStack: " << st
           << "\nRemaining-I/p: "
           << make_pair(tokens, make_pair(tokenIndex, tokens.size())) << "\n";
    }
    cout << "===\n";
  }

  return false;
}

void LL1::buildParsingTable() {
  auto& ll1ParsingTable = this->parsingTable;
  this->parsingTable.clear();
  for (const auto& symPr : this->productionRules) {
    for (ProductionRule* pr : symPr.second) {
      bool isEps = false;
      Symbol* activeSymbol = pr->lhs;
      for (Symbol* rhsSym : pr->rhs) {
        // iterate first(rhsSym)
        for (Symbol* firstRhsSym : this->firstSetsMap[rhsSym]) {
          if (firstRhsSym == this->epsSymbol) {
            isEps = true;
            continue;
          }
          // if there is already a rule in
          // ParsingTable[activeSymbol][firstRhsSym] -> throw an exception
          if (ll1ParsingTable.find(activeSymbol) != ll1ParsingTable.end() &&
              ll1ParsingTable[activeSymbol].find(firstRhsSym) !=
                  ll1ParsingTable[activeSymbol].end()) {
            throw NOT_LL1_EXCEPTION;
          }
          parsingTable[activeSymbol][firstRhsSym] = pr;
        }
        if (!isEps) break;
      }
      // if first(rhs) has eps
      if (isEps) {
        // add prodution-rule for all the input symbols in Follow(pr->lhs)
        for (Symbol* followSym : this->followSetsMap[activeSymbol]) {
          // if there is already a rule in
          // ParsingTable[activeSymbol][followSym] -> throw an exception
          if (ll1ParsingTable.find(activeSymbol) != ll1ParsingTable.end() &&
              ll1ParsingTable[activeSymbol].find(followSym) !=
                  ll1ParsingTable[activeSymbol].end()) {
            throw NOT_LL1_EXCEPTION;
          }
          parsingTable[activeSymbol][followSym] = pr;
        }
      }
    }
  }

  // print parsing table
  cout << "Parsing Table\n";
  for (const auto& nonTerEntry : this->parsingTable) {
    for (const auto& ipSymEntry : nonTerEntry.second) {
      // ipSymEntry.second
      cout << "( " << nonTerEntry.first->symbol << ", "
           << ipSymEntry.first->symbol << ") -> ";
      cout << ipSymEntry.second->lhs->symbol << " -> [ ";
      for (Symbol* rhsSym : ipSymEntry.second->rhs) {
        cout << rhsSym->symbol << " ";
      }
      cout << "]\n";
    }
  }
}

void LL1::computeFirstForSym(Symbol* sym) {
  if (this->firstSetsMap.find(sym) != firstSetsMap.end()) {
    return;
  }

  auto& firstSet = this->firstSetsMap[sym];

  if (sym->isTerminal) {
    firstSet.insert(sym);
    return;
  }

  // for non-terminals -> iterate over each production rule
  for (ProductionRule* pr : this->productionRules[sym]) {
    bool isEps = false;
    // compute first of the first sym on RHS
    // if it contains eps -> compute first of the next sym and so on...
    for (Symbol* rhsSym : pr->rhs) {
      this->computeFirstForSym(rhsSym);

      const auto& rhsFirstSet = this->firstSetsMap[rhsSym];
      for (Symbol* rhsFirstSym : rhsFirstSet) {
        firstSet.insert(rhsFirstSym);
      }
      isEps = (rhsFirstSet.find(epsSymbol) != rhsFirstSet.end());
      if (!isEps) {
        break;
      }
    }
    if (isEps) {
      firstSet.insert(this->epsSymbol);
    }
  }
}

void LL1::computeFirst() {
  cout << "First-sets: \n";
  for (Symbol* ter : this->terminals) {
    this->computeFirstForSym(ter);
    cout << ter->symbol << ": [ ";
    for (Symbol* sym : this->firstSetsMap[ter]) {
      cout << sym->symbol << " ";
    }
    cout << "]\n";
  }
  for (Symbol* nonTer : this->nonTerminals) {
    this->computeFirstForSym(nonTer);
    cout << nonTer->symbol << ": [ ";
    for (Symbol* sym : this->firstSetsMap[nonTer]) {
      cout << sym->symbol << " ";
    }
    cout << "]\n";
  }
}

void LL1::computeFollow() {
  unordered_map<Symbol*, unordered_set<Symbol*>> dependents;

  this->followSetsMap[this->startSymbol].insert(this->dollarSymbol);

  // parse all the production-rules once
  for (const auto& symRules : this->productionRules) {
    for (const ProductionRule* pr : symRules.second) {
      unordered_set<Symbol*> activeSyms;
      bool isEps = false;
      for (Symbol* rhsSym : pr->rhs) {
        if (rhsSym->isTerminal) {
          isEps = (rhsSym == this->epsSymbol);
          if (!isEps) {
            for (Symbol* activeSym : activeSyms)
              this->followSetsMap[activeSym].insert(rhsSym);
          }
        } else {
          auto firstSetOfRhsSym = this->firstSetsMap[rhsSym];
          isEps = false;
          for (Symbol* firstOfRhs : firstSetOfRhsSym) {
            isEps = (firstOfRhs == this->epsSymbol);
            if (!isEps) {
              for (Symbol* activeSym : activeSyms)
                this->followSetsMap[activeSym].insert(firstOfRhs);
            }
          }
        }
        if (!isEps) activeSyms.clear();
        if (!rhsSym->isTerminal) activeSyms.insert(rhsSym);
      }
      // -> Follow sets of all the active-syms (at the end) contain
      // Follow(lhsSym)
      //    (This rule can introduce cycles and complicate things)
      // -> For now let't just store it as a fact that "the follow of
      // active-syms depend on lhsSym"
      // (i.e., dependents[lhsSym] = active-syms)
      if (activeSyms.size() > 0) {
        dependents[pr->lhs] = activeSyms;
      }
    }
  }

  // Now we must handle the dependencies
  // First apply all the dependencies
  // i.e. if dependents(X) contains A1, A2, ... Ak,
  // make Follow(Ai) = Follow(Ai) UNION Follow(X) for all i = 1, 2, ..., k
  // Keep repeating this step till there is no change in any of the follow-sets
  bool didFollowSetUpdate = true;
  while (didFollowSetUpdate) {
    didFollowSetUpdate = false;
    for (auto& dependent : dependents) {
      for (Symbol* depSym : dependent.second) {
        for (Symbol* followSymLhs : this->followSetsMap[dependent.first])
          if (this->followSetsMap[depSym].insert(followSymLhs).second)
            didFollowSetUpdate = true;
      }
    }
  }

  // print follow sets
  cout << "Follow-sets\n";
  for (Symbol* nonTer : this->nonTerminals) {
    cout << nonTer->symbol << ": [ ";
    for (Symbol* sym : this->followSetsMap[nonTer]) {
      cout << sym->symbol << " ";
    }
    cout << "]\n";
  }
}

void LL1::eliminateLeftRecursion() {
  for (int i = 0; i < this->nonTerminals.size(); ++i) {
    auto& iProdRulesSet = this->productionRules[this->nonTerminals[i]];

    for (int j = 0; j < i; ++j) {
      // if prod-rule of Ai contains Aj in the beginning of the rule
      // replace Aj by all the productions of Aj
      vector<ProductionRule*> prsToBeRemoved;

      for (ProductionRule* pr : iProdRulesSet) {
        if (pr->rhs.size() > 0 && pr->rhs[0]->id == this->nonTerminals[j]->id) {
          prsToBeRemoved.push_back(pr);
        }
      }

      for (ProductionRule* pr : prsToBeRemoved) {
        for (ProductionRule* jPR :
             this->productionRules[this->nonTerminals[j]]) {
          vector<Symbol*> newRhs(jPR->rhs);
          for (int k = 1; k < pr->rhs.size(); ++k) {
            newRhs.push_back(pr->rhs[k]);
          }
          iProdRulesSet.insert(
              new ProductionRule(this->nonTerminals[i], newRhs));
        }
        delete pr;
        iProdRulesSet.erase(pr);
      }
    }

    // remove left recursion from Ai's production-rules
    vector<ProductionRule*> leftRecursiveRules, nonLeftRecRules;
    for (ProductionRule* pr : iProdRulesSet) {
      if (pr->rhs.size() > 0 && pr->rhs[0]->id == this->nonTerminals[i]->id) {
        leftRecursiveRules.push_back(pr);
      } else {
        nonLeftRecRules.push_back(pr);
      }
    }

    if (leftRecursiveRules.size() > 0) {
      // add new non-terminal A'
      int newSymId = this->totNumSyms++;
      Symbol* newSym =
          new Symbol(newSymId, this->nonTerminals[i]->symbol + "_'", false);
      this->nonTerminals.push_back(newSym);

      for (ProductionRule* rule : leftRecursiveRules) {
        if (rule->rhs.size() == 1) continue;
        // erase Ai from the beginning and append A' to the end of each rule
        vector<Symbol*> newRhs;
        for (int k = 1; k < rule->rhs.size(); ++k) {
          newRhs.push_back(rule->rhs[k]);
        }
        newRhs.push_back(newSym);
        this->productionRules[newSym].insert(
            new ProductionRule(newSym, newRhs));
      }

      vector<Symbol*> epsRhs{this->epsSymbol};
      this->productionRules[newSym].insert(new ProductionRule(newSym, epsRhs));

      for (ProductionRule* rule : nonLeftRecRules) {
        rule->rhs.push_back(newSym);
      }

      // delete all the production-rules in leftRecursiveRules
      for (ProductionRule* rule : leftRecursiveRules) {
        delete rule;
        iProdRulesSet.erase(rule);
      }
    }
  }
}

void LL1::leftFactor() {
  for (int i = 0; i < this->nonTerminals.size(); ++i) {
    Symbol* nonTer = this->nonTerminals[i];

    // propagate prefix map by first symbol on the rhs of each of the production
    // rule of the current nonTer
    unordered_map<Symbol*, unordered_set<ProductionRule*>> prefixMap;
    for (ProductionRule* rule : this->productionRules[nonTer]) {
      if (rule->rhs.size() > 0) {
        prefixMap[rule->rhs[0]].insert(rule);
      }
    }

    for (auto mapEntry : prefixMap) {
      // if two or more production rules are starting with the same symbol, we
      // need to factor those rules
      if (mapEntry.second.size() > 1) {
        auto& prs = mapEntry.second;

        // try to find the longest prefix common to all the production rules in
        // the current set (having same first symbol, hence, length of longest
        // prefix >= 1)
        vector<Symbol*> commonSyms((*(mapEntry.second.begin()))->rhs);
        for (ProductionRule* pr : prs) {
          while (pr->rhs.size() < commonSyms.size()) {
            commonSyms.pop_back();
          }
          while (commonSyms.size() > 0 && (pr->rhs[commonSyms.size() - 1] !=
                                           commonSyms[commonSyms.size() - 1])) {
            commonSyms.pop_back();
          }
        }

        // insert new symbol, which will derive all the rules with common prefix
        // "commonSyms"
        int newSymId = this->totNumSyms++;
        Symbol* newSym =
            new Symbol(newSymId, "NT_" + to_string(newSymId), false);
        this->nonTerminals.push_back(newSym);

        // propagate rules of the new symbol
        bool isEps = false;
        for (ProductionRule* pr : prs) {
          vector<Symbol*> newRhs;
          if (pr->rhs.size() == commonSyms.size()) {
            if (isEps) continue;
            // if rhs == "commonSyms" for some production rule,
            // the new symbol must derive epsilon (exactly once)
            newRhs.push_back(this->epsSymbol);
            isEps = true;
          } else {
            // new symbol has to derive {rhs-(common prefix "commonSyms")}
            for (int k = commonSyms.size(); k < pr->rhs.size(); ++k) {
              newRhs.push_back(pr->rhs[k]);
            }
          }
          this->productionRules[newSym].insert(
              new ProductionRule(newSym, newRhs));
        }

        // add new rule to nonTer
        // nonTer -> [ commonSyms newSymbol ]
        vector<Symbol*> newRhs(commonSyms);
        newRhs.push_back(newSym);
        this->productionRules[nonTer].insert(
            new ProductionRule(nonTer, newRhs));

        // remove factored rules from nonTer
        for (ProductionRule* pr : prs) {
          delete pr;
          this->productionRules[nonTer].erase(pr);
        }
      }
    }
  }
}

void LL1::readCFG() {
  this->symToPtr.clear();
  unordered_map<string, int> symToId;
  string sym;

  cout << "Note: terminal symbols and non-terminal symbols can be strings. "
          "The string must not contain either of whitespace, tab, newline, ']' "
          "and '_'. Use \""
       << EPSILON_SYMBOL << "\" as epsilon \n\n";

  // add epsilon to grammar
  this->symToPtr[EPSILON_SYMBOL] =
      new Symbol(this->totNumSyms, EPSILON_SYMBOL, true);
  symToId[EPSILON_SYMBOL] = this->totNumSyms;
  this->epsSymbol = this->symToPtr[EPSILON_SYMBOL];
  // this->terminals.push_back(this->symToPtr[EPSILON_SYMBOL]);
  this->totNumSyms++;

  //  add $ symbol to grammar
  this->symToPtr[DOLLAR_SYMBOL] =
      new Symbol(this->totNumSyms, DOLLAR_SYMBOL, true);
  symToId[DOLLAR_SYMBOL] = this->totNumSyms;
  this->dollarSymbol = this->symToPtr[DOLLAR_SYMBOL];
  // this->terminals.push_back(this->symToPtr[DOLLAR_SYMBOL]);
  this->totNumSyms++;

  int numNonTers, numTers, numProdRules;
  cout << "Enter number of non-terminals: ";
  cin >> numNonTers;
  cout << "Enter " << numNonTers << " non-terminals:\n";
  for (int i = 0; i < numNonTers; ++i) {
    cin >> sym;
    this->symToPtr[sym] = new Symbol(this->totNumSyms, sym, false);
    symToId[sym] = this->totNumSyms;
    this->nonTerminals.push_back(this->symToPtr[sym]);
    ++this->totNumSyms;
  }

  cout << "Enter number of terminals: ";
  cin >> numTers;
  cout << "Enter " << numTers << " terminals:\n";
  for (int i = 0; i < numTers; ++i) {
    cin >> sym;
    this->symToPtr[sym] = new Symbol(this->totNumSyms, sym, true);
    symToId[sym] = this->totNumSyms;
    this->terminals.push_back(this->symToPtr[sym]);
    ++this->totNumSyms;
  }

  cout << "\n===\n";
  cout << "Production rule must have the "
          "following format:\n";
  cout << "Non-terminal -> [ a space separated list of terminals and "
          "non-terminals (first and last symbols on rhs must be separated from "
          "the square brackets by at least one space)\n";
  cout << "Example: A -> [ a B C d a A ]\n";
  cout << "===\n\n";

  cout << "Enter number of production rules:\n";
  cin >> numProdRules;
  cout << "Enter " << numProdRules << " production rules:\n";
  for (int i = 0; i < numProdRules; ++i) {
    string tmpStr, lhsStr;
    // parsing the production rules

    cin >> sym;
    assert(this->symToPtr.find(sym) != this->symToPtr.end());
    assert(!this->symToPtr[sym]->isTerminal);
    Symbol* lhs = this->symToPtr[sym];
    lhsStr = sym;

    cin >> tmpStr;
    assert(tmpStr == "->");

    cin >> tmpStr;
    assert(tmpStr == "[");
    cin >> sym;
    vector<Symbol*> rhs;
    while (sym != "]") {
      assert(this->symToPtr.find(sym) != this->symToPtr.end());
      rhs.push_back(this->symToPtr[sym]);
      cin >> sym;
    }

    this->productionRules[this->symToPtr[lhsStr]].insert(
        new ProductionRule(lhs, rhs));
  }

  cout << "Enter start symbol: ";
  cin >> sym;
  assert(this->symToPtr.find(sym) != this->symToPtr.end());
  assert(!this->symToPtr[sym]->isTerminal);
  this->startSymbol = this->symToPtr[sym];
}

void LL1::printCFG() {
  cout << "=== CFG\n";
  cout << "Terminals: ";
  for (Symbol* terminal : this->terminals) {
    cout << terminal->symbol << " ";
  }
  cout << "\n";

  cout << "Non-terminals: ";
  for (Symbol* nonTerminal : this->nonTerminals) {
    cout << nonTerminal->symbol << " ";
  }
  cout << "\n";

  cout << "Start symbol: " << this->startSymbol->symbol << "\n";

  cout << "\nProduction rules:\n";
  for (auto pr : this->productionRules) {
    for (ProductionRule* productionRule : pr.second) {
      cout << productionRule << "\n";
    }
  }
  cout << "===\n";
}

ostream& operator<<(ostream& os, const Symbol* sym) {
  os << "[ " << sym->symbol << " " << sym->id << " " << sym->isTerminal
     << " ] ";
  return os;
}

ostream& operator<<(ostream& os, const ProductionRule* pr) {
  os << pr->lhs->symbol << " -> [ ";
  for (Symbol* rhsSym : pr->rhs) {
    os << rhsSym->symbol << " ";
  }
  os << "] ";
  return os;
}

ostream& operator<<(ostream& os, stack<Symbol*> st) {
  os << "[ ";
  while (!st.empty()) {
    os << st.top()->symbol << " ";
    st.pop();
  }
  os << "] ";
  return os;
}

ostream& operator<<(ostream& os,
                    const pair<vector<string>, pair<int, int>>& vec) {
  os << "[ ";
  for (int i = vec.second.first; i < vec.second.second; ++i) {
    os << vec.first[i] << " ";
  }
  os << "] ";
  return os;
}