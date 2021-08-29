#ifndef __COMMON_CONSTANTS_HPP__
#define __COMMON_CONSTANTS_HPP__

#include <string>
using namespace std;

const string EPSILON_SYMBOL = "ε";
const string DOLLAR_SYMBOL = "$";
const string NOT_LL1_EXCEPTION = "The CFG is not an LL(1) grammar";
const string PRODUCTION_RULE_OR = "|";

const string FIRST_FOLLOW_FILE_NAME = "First-Follow.txt";
const string LL1_GRAMMAR_FILE_NAME = "LL1Grammar.txt";
const string PARSE_TABLE_FILE_NAME = "ParseTable.txt";

#endif