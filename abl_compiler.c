#include "abl_compiler.h"

static void error_at(compiler* p, const char* msg, ...)
{
	ABL_DEBUG_DIAGNOSTIC("[line %d] error ", p->current.line);
	p->had_error = true;
	
	if (p->current == TK_EOF)
	{
		ABL_DEBUG_DIAGNOSTIC("end of file");
	} 
	else if (p->current == TK_ERR) 
	{
	}
	else
	{
		ABL_DEBUG_DIAGNOSTIC("at '%*.s'", p->current.length, p->current.start);
	}

	va_list args;
	va_start(args, msg);
	ABL_DEBUG_VDIAGNOSTIC(msg, args);
	va_end(args);
}

static int make_constant(compiler* c, abl_value val)
{
	abl_value_array_add(c->constants, val);
	return c->constants.size;
}

static void emit_constant(compiler* c, abl_value val)
{
	write_chunk(c->out, OP_CONST);
	write_chunk(c->out, make_constant(val));
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

static void grouping(compiler* c)
{
	exression();
	consume(TK_CLOSE_PAREN);
}

static void unary(compiler* c)
{
	
}

static void primary(compiler* c)
{
	switch(c->current.type)
	{
		case TK_INT:
			write_chunk(c->out, OP_PUSH);
			write4_chunk(c->out, token_as_int(&c->lex, c->current));
	}
}

static void assignement(compiler* c)
{
	
}

static void expression(compiler* c) 
{
	
}


