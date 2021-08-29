#ifndef __lexFns_HPP__
#define __lexFns_HPP__
// extern "C" -> we want to use following fns/variables from a C code compiled using gcc (not g++)
extern "C" {
	int yylex(void);
	extern char* yytext;
	extern int line_number;
	extern char* yy_token_type;
}
#endif