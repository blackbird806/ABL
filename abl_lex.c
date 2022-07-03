#include "abl_lex.h"
#include <cctype.h>

void init_lexer(lexer* lex, const char* src)
{
	ABL_ASSERT(lex);
	lex->start = src;	
	lex->current = src;
	lex->line = 1;
}

static token make_token(token_type type)
{
	token tk;
	tk.type = type;
}

static const char* get_keyword_str_from_type(token_type t)
{
	switch(t)
	{
		case TK_IF: return "if";
		case TK_FOR: return "for";
		case TK_WHILE: return "while";
		case TK_FN: return "fn";
		case TK_IMPORT: return "import";
		case TK_TRUE: return "true";
		case TK_FALSE: return "false";
		case TK_NULL: return "null";
	}
	ABL_ASSERT(false);
}

static token_type check_keyword(const char* tk_str, int start, int len, const char* rest, token_type type)
{
	// see: https://craftinginterpreters.com/scanning-on-demand.html
	if (strlen(tk_str) == start + len && memcmp(tk_str+start, rest, len) == 0)
		return type;
	return TK_IDENTIFIER;
}

static token_type get_word_token_type(const char* tk_str)
{
	switch (sk_str[0])
	{
		case 'a':
	}
}

token lex_token(lexer* lex)
{
	ABL_ASSERT(lex);
	
	if(*lex->current == '\0')
		return make_token(TK_EOF);

	switch(*lex->current) {
		case '(': return make_token(TK_OPEN_PAREN);
		case ')': return make_token(TK_CLOSE_PAREN);
		case '{': return make_token(TK_OPEN_BRACE);
		case '}': return make_token(TK_CLOSE_BRACE);
		case ';': return make_token(TK_SEMICOLON);
		case ',': return make_token(TK_COMMA);
		case '.': return make_token(TK_DOT);
		case ':': return make_token(TK_DOUBLE_DOT);
		case '-': return make_token(TK_MINUS);
		case '+': return make_token(TK_PLUS);
		case '/': return make_token(TK_SLASH);
		case '*': return make_token(TK_STAR);
	}
	
	// identifiers or keywords
	if (isalpha(*lex->current) || *lex->current == '_')
	{
		const char* start = lex->current;
		lex->current++;
		while(isalnum(*lex->current) || *lex->current == '_')
		{
			lex->current++;
		}

		token id = make_token(TK_IDENTIFIER);
		unsigned const len = lex->current - start;
		tk.sem_info.str	= (char*)ABL_MALLOC((len+1) * sizeof(char))
		strncpy(tk.sem_info.str, start, len);
		tk.sem_info.str[len] = '\0';
	}
}

