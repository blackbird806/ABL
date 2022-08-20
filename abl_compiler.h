#ifndef ABL_COMPILER_H
#define ABL_COMPILER_H

#include "abl_lex.h"
#include "abl_bytecode.h"
#include "abl_value.h"

// one pass compiler

typedef struct compiler
{
	lexer lex;
	token current;
	
	bytecode_chunk out;
	abl_value_array constants;
	bool had_error;
} compiler;

void compile(const char* src, FILE* out);

#endif
