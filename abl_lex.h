#ifndef ABL_LEX_H
#define ABL_LEX_H 

#include "abl_core.h"

typedef enum token_type {

	// special chars
	TK_OPEN_PAREN, TK_CLOSE_PAREN,
	TK_OPEN_BRACE, TK_CLOSE_BRACE,
	TK_OPEN_BRACKET, TK_CLOSE_BRACKET,
	TK_COMMA, TK_SEMICOLON, TK_MINUS, TK_PLUS, TK_STAR, TK_DOT, TK_DOUBLE_DOT, TK_NOT,
	TK_SLASH, TK_EQUAL, TK_EQUAL_EQUAL, TK_AND, TK_AND_AND, TK_OR, TK_OR_OR,
	
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

typedef union {
	abl_float f;
	abl_int i;
	const char* str;
} semantic_info;

typedef struct token {
	token_type type;
	semantic_info sem_info;
} token;

typedef struct lexer {
	const char* start;
	const char* current;
	int line;
} lexer;

void init_lexer(lexer* lex, const char* src);
token lex_token(lexer* lex);

#endif