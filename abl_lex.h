#ifndef ABL_LEX_H
#define ABL_LEX_H 

#include "abl_core.h"

typedef enum token_type {
	TK_ERR,
	// operators and specials
	TK_OPEN_PAREN, TK_CLOSE_PAREN,
	TK_OPEN_BRACE, TK_CLOSE_BRACE,
	TK_OPEN_BRACKET, TK_CLOSE_BRACKET,
	TK_COMMA, TK_SEMICOLON, TK_MINUS, TK_PLUS, TK_STAR, TK_DOT, TK_DOUBLE_DOT, TK_NOT,
	TK_SLASH, TK_EQUAL, TK_NOT_EQUAL, TK_EQUAL_EQUAL, TK_AND, TK_AND_AND, TK_OR, TK_OR_OR,
	TK_LESS, TK_GREATER, TK_LESS_EQUAL, TK_GREATER_EQUAL,

	//
	TK_IDENTIFIER, 
	
	// literals
	TK_STRING, TK_INT, TK_FLOAT, TK_BOOL,

	// keywords
	TK_FN, TK_IF, TK_WHILE, TK_FOR, TK_TRUE, TK_FALSE,
	TK_NULL, TK_IMPORT,

	//
	TK_EOF
} token_type;

const char* token_type_to_string(token_type t);

typedef struct token {
	token_type type;
	int start, length; // view on the source
	int line;
} token;

typedef struct lexer {
	const char* start;
	const char* current;
	int line;
} lexer;

void init_lexer(lexer* lex, const char* src);
token lex_token(lexer* lex);

abl_int token_as_int(lexer* l, token t);

#endif
