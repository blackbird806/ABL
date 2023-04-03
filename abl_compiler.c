#include "abl_compiler.h"

#include <stdarg.h>

#include "abl_value.h"

// DEBUG TO REMOVE
#include "abl_vm.h"
// 

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

typedef void (*parse_fn)(abl_compiler*, bool);

typedef struct
{
	parse_fn prefix;
	parse_fn infix;
	precedence prec;
} parse_rule;

static parse_rule* get_rule(token_type type);

static void error_at(abl_compiler* c, const char* msg, ...)
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
		// @Bug this do not works
		//ABL_DEBUG_DIAGNOSTIC("at '%*.s' ", c->current.length, c->current.start);
	}

	va_list args;
	va_start(args, msg);
	ABL_DEBUG_VDIAGNOSTIC(msg, args);
	va_end(args);
	ABL_DEBUG_DIAGNOSTIC("%s\n", "");
	ABL_ASSERT(false);
}

static uint32_t make_constant(abl_compiler* c, abl_value val)
{
	abl_value_array_add(&c->constants, val);
	return c->constants.size - 1;
}

static void emit_constant(abl_compiler* c, abl_value val)
{
	write_chunk(&c->code_chunk, OP_PUSHC);
	write4_chunk(&c->code_chunk, make_constant(c, val));
}

static token advance(abl_compiler* c)
{
	c->current = lex_token(&c->lex);
	return c->current;
}

static token peek(abl_compiler* c, int i)
{
	return peek_token(&c->lex, i);
}

static void consume(abl_compiler* c, token_type t)
{
	if (advance(c).type != t)
	{
		error_at(c, "expected '%s' got '%s' instead", token_type_to_string(t), token_type_to_string(c->current.type));
	}
}

// @review https://craftinginterpreters.com/global-variables.html#error-synchronization
static void synchronize(abl_compiler* c)
{
	c->had_error = false;

	while (c->current.type != TK_EOF)
	{
		switch (c->current.type)
		{
		case TK_IF:
		case TK_FN:
		case TK_FOR:
		case TK_WHILE:
		case TK_RETURN:
			return;
		default:
			break;
		}
		advance(c);
	}
}

static void parse_precedence(abl_compiler* c, precedence prec)
{
	advance(c);
	parse_fn const prefix_rule = get_rule(c->current.type)->prefix;
	if (prefix_rule == NULL)
	{
		error_at(c, "expression expected");
		return;
	}

	bool const canAssign = prec <= PREC_ASSIGNMENT;
	prefix_rule(c, canAssign);

	while (prec <= get_rule(peek_token(&c->lex, 1).type)->prec)
	{
		advance(c);
		parse_fn const infix_rule = get_rule(c->current.type)->infix;
		infix_rule(c, canAssign);
	}

	if (canAssign && c->current.type == TK_EQUAL)
	{
		error_at(c, "invalid assignement target");
	}
}

static void expression(abl_compiler* c)
{
	parse_precedence(c, PREC_ASSIGNMENT);
}

static void grouping(abl_compiler* c, bool canAssign)
{
	expression(c);
	consume(c, TK_CLOSE_PAREN);
}

static void binary(abl_compiler* c, bool canAssign)
{
	token_type const op_type = c->current.type;
	parse_rule const* rule = get_rule(op_type);
	parse_precedence(c, rule->prec);

	switch (op_type)
	{
	case TK_PLUS:	write_chunk(&c->code_chunk, OP_ADD); break;
	case TK_MINUS:	write_chunk(&c->code_chunk, OP_SUB); break;
	case TK_STAR:	write_chunk(&c->code_chunk, OP_MUL); break;
	case TK_SLASH:	write_chunk(&c->code_chunk, OP_DIV); break;
		default:
			error_at(c, "unrecognized binary operator");
	}
}

static void unary(abl_compiler* c, bool canAssign)
{
	token_type const op_type = c->current.type;
	parse_precedence(c, PREC_UNARY);

	switch (op_type)
	{
	case TK_MINUS:
		write_chunk(&c->code_chunk, OP_NEG);
		break;
	case TK_NOT:
		write_chunk(&c->code_chunk, OP_NOT);
		break;
	case TK_PLUS: // unary plus is implicit so we do nothing here
		break;
	default:
		error_at(c, "unary operator not recognized");
		break;
	}
}

static void int_literal(abl_compiler* c, bool canAssign)
{
	emit_constant(c, make_int(token_as_int(&c->lex, c->current)));
}

static void float_literal(abl_compiler* c, bool canAssign)
{
	//emit_constant(c, make_float(token_as_float(&c->lex, c->current)));
	ABL_ASSERT(false);
}

static void string_literal(abl_compiler* c, bool canAssign)
{
	emit_constant(c, make_string(token_as_string(&c->lex, c->current)));
}

static void bool_literal(abl_compiler* c, bool canAssign)
{
	emit_constant(c, make_bool(c->current.type == TK_TRUE));
}

static void variable(abl_compiler* c, bool canAssign)
{
	write_chunk(&c->code_chunk, OP_LOAD);
	//write4_chunk(&c->code_chunk, c->constants)
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
  [TK_EQUAL] = {NULL,     NULL,   PREC_ASSIGNMENT},
  [TK_EQUAL_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_GREATER] = {NULL,     NULL,   PREC_NONE},
  [TK_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_LESS] = {NULL,     NULL,   PREC_NONE},
  [TK_LESS_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TK_IDENTIFIER] = {variable,     NULL,   PREC_NONE},
  [TK_STRING] = {string_literal,     NULL,   PREC_NONE},
  [TK_INT] = {int_literal,   NULL,   PREC_NONE},
  [TK_FLOAT] = {float_literal,   NULL,   PREC_NONE},
  [TK_AND] = {NULL,     NULL,   PREC_NONE},
  [TK_ELSE] = {NULL,     NULL,   PREC_NONE},
  [TK_FALSE] = {bool_literal,     NULL,   PREC_NONE},
  [TK_FOR] = {NULL,     NULL,   PREC_NONE},
  [TK_FN] = {NULL,     NULL,   PREC_NONE},
  [TK_IF] = {NULL,     NULL,   PREC_NONE},
  [TK_NULL] = {NULL,     NULL,   PREC_NONE},
  [TK_RETURN] = {NULL,     NULL,   PREC_NONE},
  [TK_OR] = {NULL,     NULL,   PREC_NONE},
  [TK_BOOL] = {NULL,     NULL,   PREC_NONE},
  [TK_TRUE] = {bool_literal,     NULL,   PREC_NONE},
  [TK_WHILE] = {NULL,     NULL,   PREC_NONE},
  [TK_ERR] = {NULL,     NULL,   PREC_NONE},
  [TK_EOF] = {NULL,     NULL,   PREC_NONE},
};

static parse_rule* get_rule(token_type type) {
	return &rules[type];
}

static void constant(abl_compiler* c, abl_value val)
{
	write_chunk(&c->constants_chunk, val.type);
	switch (val.type)
	{
	case VAL_INT:
		write4_chunk(&c->constants_chunk, val.v.i);
		break;
	case VAL_BOOL:
		write_chunk(&c->constants_chunk, val.v.b);
		break;
	case VAL_OBJ:
		write_chunk(&c->constants_chunk, val.v.o->type);
		switch (val.v.o->type)
		{
		case OBJ_STRING:
			write4_chunk(&c->constants_chunk, ((abl_string*)val.v.o)->size);
			writestr_chunk(&c->constants_chunk, (abl_string*)val.v.o);
		break;
		default:
			error_at(c, "constant not recognised");
		break;
		}
		break;
	default:
		error_at(c, "constant not recognised");
		break;
	}
}

static void expr_statement(abl_compiler* c)
{
	expression(c);
	consume(c, TK_SEMICOLON);
	write_chunk(&c->code_chunk, OP_POP);
}

static void declaration(abl_compiler* c);

static void block_statement(abl_compiler* c)
{
	consume(c, TK_OPEN_BRACE);
	while (peek(c, 1).type != TK_CLOSE_BRACE)
	{
		declaration(c);
	}
	advance(c); // pass close bracket
}

static void if_statement(abl_compiler* c)
{
	consume(c, TK_IF);
	expression(c);
	block_statement(c);
}

static void return_statement(abl_compiler* c)
{
	consume(c, TK_RETURN);
	expression(c);
	consume(c, TK_SEMICOLON);
}

static void statement(abl_compiler* c)
{
	token_type const next = peek(c, 1).type;
	switch(next)
	{
	case TK_IF:
		if_statement(c);
		break;
	case TK_RETURN:
		return_statement(c);
		break;
	case TK_OPEN_BRACE:
		block_statement(c);
		break;
		default:
			expr_statement(c);
	}
}

static uint32_t parse_variable(abl_compiler* c)
{
	uint32_t const const_id = make_constant(c, make_string(token_as_string(&c->lex, c->current)));
	return const_id;
}

static void var_assignement(abl_compiler* c)
{
	uint32_t const global = parse_variable(c);
	consume(c, TK_EQUAL);
	expression(c);
	consume(c, TK_SEMICOLON);
	write_chunk(&c->code_chunk, OP_STORE);
	write4_chunk(&c->code_chunk, global);
}

static void declaration(abl_compiler* c)
{
	advance(c);
	if (c->current.type == TK_IDENTIFIER && peek_token(&c->lex, 1).type == TK_EQUAL)
		var_assignement(c);
	else
		statement(c);

	if (c->had_error)
		synchronize(c);
}

static void compile_constants(abl_compiler* c)
{
	write_chunk(&c->constants_chunk, SECTION_CONSTANTS);
	write4_chunk(&c->constants_chunk, c->constants.size);
	for (uint32_t i = 0; i < c->constants.size; i++)
	{
		write4_chunk(&c->constants_chunk, i);
		constant(c, c->constants.values[i]);
	}
}

void compile(const char* src, FILE* out)
{
	abl_compiler c;
	init_lexer(&c.lex, src);
	abl_value_array_init(&c.constants);
	init_chunk(&c.code_chunk);
	init_chunk(&c.constants_chunk);
	c.had_error = false;
	write_chunk(&c.code_chunk, SECTION_CODE);
	statement(&c);
	compile_constants(&c);

	disassemble_chunk(&c.constants_chunk, out);
	disassemble_chunk(&c.code_chunk, out);

	for (int i = 0; i < c.code_chunk.size; i++)
	{
		if (i % 16 == 0)
			printf("\n");
		printf("%02X ", c.code_chunk.code[i]);
	}

	abl_vm vm;
	abl_vm_init(&vm);
	abl_vm_get_constants_from_compiler(&vm, &c);

	abl_vm_interpret(&vm, &c.code_chunk);

	abl_vm_destroy(&vm);

}
