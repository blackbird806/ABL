#include "abl_parse.h"

static void error_at(parser* p, const char* msg)
{
	ABL_DEBUG_DIAGNOSTIC("[line %d] error ", p->current.line);
	
}

decl parse_decl(parser* p)
{
	ABL_ASSERT(p);
}
