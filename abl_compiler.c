#include "abl_compiler.h"

#include <stdarg.h>

#include "abl_value.h"

typedef enum {
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_EQUALITY,    // == !=
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_UNARY,       // ! -
	PREC_CALL,        // . ()
	PREC_PRIMARY
} precedence;

typedef void (*parse_fn)(compiler*);

typedef struct
{
	parse_fn prefix;
	parse_fn infix;
	precedence prec;
} parse_rule;

static parse_rule* get_rule(token_type type);

static void error_at(compiler* c, const char* msg, ...)
{
	ABL_DEBUG_DIAGNOSTIC("[line %d] error ", c->current.line);
	c->had_error = true;
	
	if (c->current.type == TK_EOF)
	{
		// @TODO do this cleanly
		ABL_DEBUG_DIAGNOSTIC("end of file %s", "");
	} 
	else if (c->current.type == TK_ERR) 
	{
	}
	else
	{
		ABL_DEBUG_DIAGNOSTIC("at '%*.s' ", c->current.length, c->current.start);
	}

	va_list args;
	va_start(args, msg);
	ABL_DEBUG_VDIAGNOSTIC(msg, args);
	va_end(args);
	ABL_DEBUG_DIAGNOSTIC("%s\n", "");
	__debugbreak();
}

static uint32_t make_constant(compiler* c, abl_value val)
{
	abl_value_array_add(&c->constants, val);
	return c->constants.size - 1;
}

static void emit_constant(compiler* c, abl_value val)
{
	write_chunk(&c->out, OP_CONST);
	write4_chunk(&c->out, make_constant(c, val));
}

static token advance(compiler* c)
{
	c->current = lex_token(&c->lex);
	return c->current;
}

static void consume(compiler* c, token_type t)
{
	if (advance(c).type != t)
	{
		error_at(c, "expected '%s' got '%s' instead", token_type_to_string(t), token_type_to_string(c->current.type));
	}
}

static void parse_precedence(compiler* c, precedence prec)
{
	advance(c);
	parse_fn const prefix_rule = get_rule(c->current.type)->prefix;
	if (prefix_rule == NULL)
	{
		error_at(c, "expression expected");
		return;
	}
	prefix_rule(c);

	while (prec <= get_rule(peek_token(&c->lex, 1).type)->prec)
	{
		advance(c);
		parse_fn const infix_rule = get_rule(c->current.type)->infix;
		infix_rule(c);
	}
}

static void expression(compiler* c)
{
	parse_precedence(c, PREC_ASSIGNMENT);
}

static void grouping(compiler* c)
{
	expression(c);
	consume(c, TK_CLOSE_PAREN);
}

static void binary(compiler* c)
{
	token_type const op_type = c->current.type;
	parse_rule const* rule = get_rule(op_type);
	parse_precedence(c, rule->prec);

	switch (op_type)
	{
	case TK_PLUS: write_chunk(&c->out, OP_ADD); break;
	case TK_MINUS: write_chunk(&c->out, OP_SUB); break;
	case TK_STAR: write_chunk(&c->out, OP_MUL); break;
	case TK_SLASH: write_chunk(&c->out, OP_DIV); break;
		default:
			error_at(c, "unrecognized binary operator");
	}
}

static void unary(compiler* c)
{
	token_type const op_type = c->current.type;
	parse_precedence(c, PREC_UNARY);

	switch (op_type)
	{
	case TK_MINUS:
		write_chunk(&c->out, OP_NEG);
		break;
	case TK_NOT:
		write_chunk(&c->out, OP_NOT);
		break;
	case TK_PLUS: // unary plus is implicit so we do nothing here
		break;
	default:
		error_at(c, "unary operator not recognized");
		break;
	}
}

static void primary(compiler* c)
{
	switch(c->current.type)
	{
		case TK_INT:
			emit_constant(c, make_int(token_as_int(&c->lex, c->current)));
		break;
		case TK_TRUE:
		case TK_FALSE:
			emit_constant(c, make_bool(token_as_bool(&c->lex, c->current)));
			break;
		default:
			ABL_ASSERT(false); // unreachable
		break;
	}
}

parse_rule rules[] = {
  [TK_OPEN_PAREN] = {grouping, NULL,   PREC_NONE},
  [TK_CLOSE_PAREN] = {NULL,     NULL,   PREC_NONE},
  [TK_OPEN_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TK_CLOSE_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TK_COMMA] = {NULL,     NULL,   PREC_NONE},
  [TK_DOT] = {NULL,     NULL,   PREC_NONE},
  [TK_MINUS] = {unary,    binary, PREC_TERM},
  [TK_PLUS] = {unary,     binary, PREC_TERM},
  [TK_SEMICOLON] = {NULL,     NULL,   PREC_NONE},
  [TK_SLASH] = {NULL,     binary, PREC_FACTOR},
  [TK_STAR] = {NULL,     binary, PREC_FACTOR},
  [TK_NOT] = {unary,     NULL,   PREC_NONE},
  [TK_NOT_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_EQUAL_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_GREATER] = {NULL,     NULL,   PREC_NONE},
  [TK_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_LESS] = {NULL,     NULL,   PREC_NONE},
  [TK_LESS_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_IDENTIFIER] = {NULL,     NULL,   PREC_NONE},
  [TK_STRING] = {NULL,     NULL,   PREC_NONE},
  [TK_INT] = {primary,   NULL,   PREC_NONE},
  [TK_FLOAT] = {primary,   NULL,   PREC_NONE},
  [TK_AND] = {NULL,     NULL,   PREC_NONE},
  [TK_ELSE] = {NULL,     NULL,   PREC_NONE},
  [TK_FALSE] = {NULL,     NULL,   PREC_NONE},
  [TK_FOR] = {NULL,     NULL,   PREC_NONE},
  [TK_FN] = {NULL,     NULL,   PREC_NONE},
  [TK_IF] = {NULL,     NULL,   PREC_NONE},
  [TK_NULL] = {NULL,     NULL,   PREC_NONE},
  [TK_RETURN] = {NULL,     NULL,   PREC_NONE},
  [TK_OR] = {NULL,     NULL,   PREC_NONE},
  [TK_BOOL] = {NULL,     NULL,   PREC_NONE},
  [TK_TRUE] = {NULL,     NULL,   PREC_NONE},
  [TK_WHILE] = {NULL,     NULL,   PREC_NONE},
  [TK_ERR] = {NULL,     NULL,   PREC_NONE},
  [TK_EOF] = {NULL,     NULL,   PREC_NONE},
};

static parse_rule* get_rule(token_type type) {
	return &rules[type];
}

static void constant(compiler* c, abl_value val)
{
	write_chunk(&c->out, val.type);
	switch (val.type)
	{
	case VAL_INT:
		write4_chunk(&c->out, val.v.i);
		break;
	case VAL_BOOL:
		write_chunk(&c->out, val.v.b);
		break;
	default:
		error_at(c, "constant not recognised");
		break;
	}
}

static void compile_constants(compiler* c)
{
	write_chunk(&c->out, SECTION_CONSTANTS);
	for (uint32_t i = 0; i < c->constants.size; i++)
	{
		write4_chunk(&c->out, i);
		constant(c, c->constants.values[i]);
	}
}

void compile(const char* src, FILE* out)
{
	compiler c;
	init_lexer(&c.lex, src);
	abl_value_array_init(&c.constants);
	init_chunk(&c.out);
	c.had_error = false;
	expression(&c);
	compile_constants(&c);
	disassemble_chunk(&c.out, out);

	for (int i = 0; i < c.out.size; i++)
	{
		if (i % 16 == 0)
			printf("\n");
		printf("%02X ", c.out.code[i]);
	}
}
