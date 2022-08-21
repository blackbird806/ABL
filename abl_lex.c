#include "abl_lex.h"
#include <ctype.h>
#include <string.h>

const char* token_type_to_string(token_type t)
{
#define TK_STR(T) [T] = #T
	static const char* str_values[]	= {
		TK_STR(TK_ERR),
		TK_STR(TK_OPEN_PAREN),
		TK_STR(TK_CLOSE_PAREN),
		TK_STR(TK_OPEN_BRACE),
		TK_STR(TK_CLOSE_BRACE),
		TK_STR(TK_OPEN_BRACKET),
		TK_STR(TK_CLOSE_BRACKET),
		TK_STR(TK_COMMA),
		TK_STR(TK_SEMICOLON),
		TK_STR(TK_MINUS),
		TK_STR(TK_PLUS),
		TK_STR(TK_STAR),
		TK_STR(TK_DOT),
		TK_STR(TK_DOUBLE_DOT),
		TK_STR(TK_NOT),
		TK_STR(TK_SLASH),
		TK_STR(TK_EQUAL),
		TK_STR(TK_NOT_EQUAL),
		TK_STR(TK_EQUAL_EQUAL),
		TK_STR(TK_AND),
		TK_STR(TK_AND_AND),
		TK_STR(TK_OR),
		TK_STR(TK_OR_OR),
		TK_STR(TK_LESS),
		TK_STR(TK_GREATER),
		TK_STR(TK_LESS_EQUAL),
		TK_STR(TK_GREATER_EQUAL),
		TK_STR(TK_IDENTIFIER),
		TK_STR(TK_STRING),
		TK_STR(TK_INT),
		TK_STR(TK_FLOAT),
		TK_STR(TK_BOOL),
		TK_STR(TK_FN),
		TK_STR(TK_IF),
		TK_STR(TK_ELSE),
		TK_STR(TK_WHILE),
		TK_STR(TK_FOR),
		TK_STR(TK_TRUE),
		TK_STR(TK_FALSE),
		TK_STR(TK_NULL),
		TK_STR(TK_IMPORT),
		TK_STR(TK_RETURN),
		TK_STR(TK_EOF),
	};
	ABL_ASSERT((int)t < ABL_ARRAY_SIZE(str_values));
	return str_values[(int)t];
}

abl_int token_as_int(lexer* lex, token t)
{
	return (abl_int)strtol(lex->src + t.start, NULL, 10);
}

abl_bool token_as_bool(lexer* lex, token t)
{
	return (abl_bool)strncmp(lex->src + t.start, "true", 4) > 0;
}

void init_lexer(lexer* lex, const char* src)
{
	ABL_ASSERT(lex);
	lex->src = src;
	lex->start = src;
	lex->current = src;
	lex->line = 1;
}

static token make_token(lexer* lex, token_type type)
{
	token tk;
	tk.type = type;
	tk.start = (int)(lex->start - lex->src);
	tk.length = (int)(lex->current - lex->start);
	tk.line = lex->line;
	return tk;
}

static bool is_at_end(lexer* lex)
{
	return *lex->current == '\0';
}

static void skip_whitespace(lexer* lex) 
{
	while (1)
	{
		switch (*lex->current)
		{
			case '\n':
			   lex->line++;	
			case ' ':
			case '\t': 
			case '\r': 
			   lex->current++;
			break;
			// also skip comments here
			case '/':
				if (*(lex->current+1) == '/' )
				{
					while (*lex->current != '\n' && !is_at_end(lex))
						lex->current++;
				}
				else
					return;
			default: 
				return;
		}
	}
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
	return TK_IDENTIFIER;
}

static token lex_string(lexer* lex)
{
	while (*lex->current != '"' && !is_at_end(lex))
	{
		if (*lex->current == '\n') 
			lex->line++;
		lex->current++;
	}
	if (is_at_end(lex))
	{
		// TODO error handling
		ABL_DEBUG_DIAGNOSTIC("lexer error : lexing string line %d\n", lex->line);
		ABL_ASSERT(false);
	}
	lex->current++; // pass the second '"'
	return make_token(lex, TK_STRING);
}

static token lex_num(lexer* lex)
{
	while (isdigit(*lex->current))
		lex->current++;
	
	bool is_float = false;
	if (*lex->current == '.')
	{
		is_float = true;
		lex->current++;
		while (isdigit(*lex->current))
			lex->current++;
	}

	return is_float ? make_token(lex, TK_FLOAT) : make_token(lex, TK_INT);
}

static token lex_identifier_or_keyword(lexer* lex)
{
	const char* start = lex->current;
	lex->current++;
	while(isalnum(*lex->current) || *lex->current == '_')
		lex->current++;

	return make_token(lex, get_word_token_type(lex));
}

token lex_token(lexer* lex)
{
	ABL_ASSERT(lex);
	
	skip_whitespace(lex);
	if(*lex->current == '\0')
		return make_token(lex, TK_EOF);

	lex->start = lex->current;
	char const c = *lex->current++;
	printf("[lexer] current char %c\n", c);
	switch(c) {
		case '(': return make_token(lex, TK_OPEN_PAREN);
		case ')': return make_token(lex, TK_CLOSE_PAREN);
		case '{': return make_token(lex, TK_OPEN_BRACE);
		case '}': return make_token(lex, TK_CLOSE_BRACE);
		case ';': return make_token(lex, TK_SEMICOLON);
		case ',': return make_token(lex, TK_COMMA);
		case '.': return make_token(lex, TK_DOT);
		case ':': return make_token(lex, TK_DOUBLE_DOT);
		case '-': return make_token(lex, TK_MINUS);
		case '+': return make_token(lex, TK_PLUS);
		case '/': return make_token(lex, TK_SLASH);
		case '*': return make_token(lex, TK_STAR);
		case '"': return lex_string(lex);
		case '!': return make_token(lex, match(lex, '=') ? TK_NOT_EQUAL : TK_NOT); 
		case '=': return make_token(lex, match(lex, '=') ? TK_EQUAL_EQUAL : TK_EQUAL); 
		case '<': return make_token(lex, match(lex, '=') ? TK_LESS_EQUAL : TK_LESS); 
		case '>': return make_token(lex, match(lex, '=') ? TK_GREATER_EQUAL : TK_GREATER); 
	}

	if (isdigit(c))
		return lex_num(lex);

	// identifiers or keywords
	if (isalpha(c) || c == '_')
	{
		return lex_identifier_or_keyword(lex);	
	}

	return make_token(lex, TK_ERR);
}

