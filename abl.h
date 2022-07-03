#ifndef ABL_H
#define ABL_H

#include "abl_lex.h"

typedef struct parser
{
	lexer lex;
} parser;

typedef struct abl_ctx
{
	
} abl_ctx;

void intepret(abl_ctx* ctx, const char* src);

#endif
