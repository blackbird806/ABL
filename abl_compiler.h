#ifndef ABL_COMPILER_H
#define ABL_COMPILER_H

#include "abl_lex.h"
#include "abl_bytecode.h"
#include "abl_value.h"

#define MAX_LOCALS 256

// one pass compiler

typedef struct
{
	token name;
	int depth;
} local;

typedef struct {
	local locals[MAX_LOCALS];
	int local_count;
	int scope_depth;
} frame;

typedef struct
{
	lexer lex;
	token current;
	
	bytecode_chunk constants_chunk;
	bytecode_chunk code_chunk;
	abl_value_array constants;
	frame current_frame;

	bool had_error;
} abl_compiler;

void compile(const char* src, FILE* out);

#endif
