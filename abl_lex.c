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

static token_type check_keyword(lexer* lex, int start, int len, const char* rest, token_type type)
{
	// see: https://craftinginterpreters.com/scanning-on-demand.html
	if (lex->current - lex->start == start + len && memcmp(lex->start + start, rest, len) == 0)
		return type;
	return TK_IDENTIFIER;
}

static bool is_at_end(lexer* lex)
{
	return *lex->current == '\0';
}

static bool match(lexer* lex, char expected)
{
	if (is_at_end(lex)) return false;

	if (*lex->current != expected) return false;
	lex->current++;
	return true;
}

static token_type get_word_token_type(lexer* lex)
{
	switch (lex->start[0])
	{
		case 'i':
		   	if (lex->current - lex->start > 1)	
			{
				switch (lex->start[1])
				{
					case 'f': return TK_IF;
					case 'm': check_keyword(lex, 2, 4, "port", TK_IMPORT);
				}
			}
		break;
		case 'f':
			if (lex->current - lex->start > 1)
			{
				switch (lex->start[1])
				{
					case 'a': return check_keyword(lex, 2, 3, "lse", TK_FALSE);
					case 'o': return check_keyword(lex, 2, 1, "r", TK_FOR);
					case 'n': return TK_FN;
				}
			}
		break;
		case 'w': return check_keyword(lex, 1, 4, "hile", TK_WHILE);
		case 'n': return check_keyword(lex, 1, 3, "ull", TK_NULL);
		case 't': return check_keyword(lex, 1, 3, "rue", TK_TRUE);
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
		case '!': return make_token(match(lex, '=') ? TK_NOT_EQUAL : TK_NOT); 
		case '=': return make_token(match(lex, '=') ? TK_EQUAL_EQUAL : TK_EQUAL); 
		case '<': return make_token(match(lex, '=') ? TK_LESS_EQUAL : TK_LESS); 
		case '>': return make_token(match(lex, '=') ? TK_GREATER_EQUAL : TK_GREATER); 
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

