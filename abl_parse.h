#ifndef ABL_PARSE_H
#define ABL_PARSE_H

#include "abl_lex.h"

/*
 *	ABL grammar
 * 
 * 	This grammar is approximative (and incomplete) and may not reflect the real language grammar
 * 	it is used to help implementation.
 *
 *  decl ::= fn_decl | var_decl | stmt
 *	var_decl ::= "var" IDENTIFIER ("=" exp) ";"
 *	fn_decl ::= "fn" IDENTIFIER "(" parameters* ")" block
 *	parameters ::= IDENTIFIER ( "," IDENTIFIER)*
 *
 *	stmt ::= expr_stmt | if | return | while | block
 *	expr_stmt ::= expr ";"
 *	if ::= "if" expr block
 *	return ::= "return" expr ";"
 *	block ::= "{" decl* "}"
 *
 *	expr ::= assignement | primary | expr binop expr | unop expr
 *	assignement ::= IDENTIFIER "=" expr
 *	primary ::= "true" | "false" | "null" | NUMBER | STRING | IDENTIFIER
 *	
 * */

typedef struct parser
{
	lexer lex;
	token current;

	bool had_error;
} parser;

typedef struct block_stmt
{
	struct decl* decl_list;
	unsigned decl_count;
} block_stmt;

typedef struct fn_decl
{
	const char* name;
	const char** params_names;
	unsigned param_count;
	block_stmt body;
} fn_decl;

typedef struct var_decl 
{
	const char* name;
//	expr expression;
} var_decl;

typedef enum decl_type
{
	DECL_FN,
	DECL_VAR,
} decl_type;

typedef struct decl {
	union {
	fn_decl fn;
	var_decl var;
	} d;
	decl_type type;
} decl;

#endif
