%{
#include <stdio.h>
#define ID_TOKEN 1
#define SEPERATOR_TOKEN 2
#define ARITH_OP_TOKEN 3
#define ERROR_TOKEN -1
int line_number = 1;
char* yy_token_type = "";
%}

whitespace [ \t]
seperator [()]

digit [0-9]
letter [a-zA-Z]
l_or_d  ({letter}|{digit})
identifier ({letter}{l_or_d}*)

arithmetic_op [+*]

%%
{identifier} {yy_token_type="id"; return ID_TOKEN;}
{arithmetic_op} {yy_token_type=yytext; return ARITH_OP_TOKEN;}
{seperator} {yy_token_type=yytext; return SEPERATOR_TOKEN;}

{whitespace} {;}
\n {line_number++;}
. {return ERROR_TOKEN;}
%%

int yywrap(void){return 1;}