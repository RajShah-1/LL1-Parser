#include "LL1Parser.hpp"

LL1Parser::LL1Parser(const string& path) {
  this->totNumSyms = 0;
  this->dirPath = (path != "") ? path : ".";

  ifstream inFileLL1;
  inFileLL1.open(this->dirPath + "/" + LL1_GRAMMAR_FILE_NAME);
  // read LL1 grammar from the file
  this->readLL1File(inFileLL1);
  inFileLL1.close();

  // read parsing table from the file

  ifstream inFileParseTable;
  inFileParseTable.open(this->dirPath + "/" + PARSE_TABLE_FILE_NAME);
  this->readParseTableFile(inFileParseTable);
  inFileParseTable.close();
}

bool LL1Parser::predictiveParsing(const vector<string>& tokens) const {
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

void LL1Parser::readParseTableFile(istream& in) {
  string iStr;
  in >> iStr;
  assert(iStr == "===Parse-table-begin:");

  int numEntries;
  in >> numEntries;
  string nonTer, inpSym;
  for (int i = 0; i < numEntries; ++i) {
    in >> iStr;
    assert(iStr == "Parse[");
    in >> nonTer >> inpSym;
    assert(this->symToPtr.find(nonTer) != this->symToPtr.end());
    assert(this->symToPtr.find(inpSym) != this->symToPtr.end());
    in >> iStr;
    assert(iStr == "]");
    in >> iStr;
    assert(iStr == ":");

    // read production rule
    string tmpStr, lhsStr, sym;
    // parsing the production rules

    in >> sym;
    assert(this->symToPtr.find(sym) != this->symToPtr.end());
    assert(!this->symToPtr[sym]->isTerminal);
    Symbol* lhs = this->symToPtr[sym];
    lhsStr = sym;

    in >> tmpStr;
    assert(tmpStr == "->");

    in >> tmpStr;
    assert(tmpStr == "[");
    in >> sym;
    vector<Symbol*> rhs;
    while (sym != "]") {
      assert(this->symToPtr.find(sym) != this->symToPtr.end());
      rhs.push_back(this->symToPtr[sym]);
      in >> sym;
    }
    this->parsingTable[symToPtr[nonTer]][symToPtr[inpSym]] =
        new ProductionRule(lhs, rhs);
  }

  in >> iStr;
  assert(iStr == "===Parse-table-end");
}

void LL1Parser::readLL1File(istream& in) {
  this->symToPtr.clear();
  unordered_map<string, int> symToId;
  string sym, iStr;

  // Check the beginning
  in >> iStr;
  assert(iStr == "===LL1-grammar-begin:");

  // read terminals
  in >> iStr;
  assert(iStr == "Terminals:");
  int numTers;
  in >> numTers;

  for (int i = 0; i < numTers; ++i) {
    in >> sym;
    this->symToPtr[sym] = new Symbol(this->totNumSyms, sym, true);
    symToId[sym] = this->totNumSyms;
    this->nonTerminals.push_back(this->symToPtr[sym]);
    ++this->totNumSyms;
  }

  in >> iStr;
  assert(iStr == "Non-terminals:");

  int numNonTers;
  in >> numNonTers;
  for (int i = 0; i < numNonTers; ++i) {
    in >> sym;
    assert(this->symToPtr.find(sym) == this->symToPtr.end());
    this->symToPtr[sym] = new Symbol(this->totNumSyms, sym, false);
    symToId[sym] = this->totNumSyms;
    this->nonTerminals.push_back(this->symToPtr[sym]);
    ++this->totNumSyms;
  }

  in >> iStr;
  assert(iStr == "Eps-symbol:");

  in >> sym;
  assert(this->symToPtr.find(sym) == this->symToPtr.end());
  this->symToPtr[sym] = new Symbol(this->totNumSyms, sym, true);
  this->epsSymbol = this->symToPtr[sym];
  symToId[sym] = this->totNumSyms;
  ++this->totNumSyms;

  in >> iStr;
  assert(iStr == "Dollar-symbol:");

  in >> sym;
  assert(this->symToPtr.find(sym) == this->symToPtr.end());
  this->symToPtr[sym] = new Symbol(this->totNumSyms, sym, true);
  this->dollarSymbol = this->symToPtr[sym];
  symToId[sym] = this->totNumSyms;
  ++this->totNumSyms;

  in >> iStr;
  assert(iStr == "Start-symbol:");

  in >> sym;
  assert(this->symToPtr.find(sym) != this->symToPtr.end());
  assert(!this->symToPtr[sym]->isTerminal);
  this->startSymbol = this->symToPtr[sym];

  in >> iStr;
  assert(iStr == "===Production-rules-begin:");
  // ignore production rules and return
}

void LL1Parser::printCFG() const {
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
}

void LL1Parser::printParseTable() const {
  cout << "===Parse-table-begin:\n";
  for (auto nonTerRow : this->parsingTable) {
    for (auto ipSymCell : nonTerRow.second) {
      cout << "Parse[ " << nonTerRow.first->symbol << " "
           << ipSymCell.first->symbol << " ] : ";
      cout << ipSymCell.second << "\n";
    }
  }
  cout << "===Parse-table-end\n";
}

void LL1Parser::setDirPath(const string& dirPath) { this->dirPath = dirPath; }

ostream& operator<<(ostream& os, const Symbol* sym) {
  os << sym->symbol << " ";
  // os << "[ " << sym->symbol << " " << sym->id << " " << sym->isTerminal
  //    << " ] ";
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