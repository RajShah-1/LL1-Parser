%{
#include <stdio.h>
#define KEYWORD_TOKEN 1
#define STR_TOKEN 1
#define INT_TOKEN 3
#define ID_TOKEN 5
#define FLOAT_TOKEN 4
#define REL_OP_TOKEN 6
#define ARITH_OP_TOKEN 7
#define ASSIGN_OP_TOKEN 8
#define SEPERATOR_TOKEN 9
#define ERROR_TOKEN -1

int line_number = 1;
char* yy_token_type = "";

%}
whitespace [ \t]
seperator [{}();]

keyword and|else|end|if|int|do|float|not|or|print|prog|scan|then|while

digit [0-9]
digits ({digit}+)
integer ({digit}+)
float ({digits}(.{digits})?([eE][-+]?{digits})?)

letter [a-zA-Z]
l_or_d  ({letter}|{digit})
identifier ({letter}{l_or_d}*)
string \"(\\.|[^"\\])*\"

assignment_op :=
relational_op [<>=]
arithmetic_op [-+*/]

%%
{keyword} {yy_token_type=yytext;return KEYWORD_TOKEN;}
{identifier} {yy_token_type="id"; return ID_TOKEN;}

{integer} {yy_token_type="ic"; return INT_TOKEN;}
{float} {yy_token_type="fc"; return FLOAT_TOKEN;}

{assignment_op} {yy_token_type=yytext;return ASSIGN_OP_TOKEN;}
{relational_op} {yy_token_type=yytext;return REL_OP_TOKEN;}
{arithmetic_op} {yy_token_type=yytext;return ARITH_OP_TOKEN;}

{string} {yy_token_type="str";return STR_TOKEN;}
{seperator} {yy_token_type=yytext;return SEPERATOR_TOKEN;}

{whitespace} {;}
\n {line_number++;}
. {return ERROR_TOKEN;}
%%

int yywrap(void){return 1;}