#include "abl_parse.h"

static void error_at(parser* p, const char* msg)
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
	ABL_DEBUG_DIAGNOSTIC("%s\n", msg);
}

