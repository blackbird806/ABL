#ifndef ABL_H
#define ABL_H

#include "abl_parse.h"

typedef struct abl_ctx
{
	parser parser_;
} abl_ctx;

void intepret(abl_ctx* ctx, const char* src);

#endif
