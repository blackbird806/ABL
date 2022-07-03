#include "abl.h"

void interpret(abl_ctx* ctx, const char* src)
{
	ABL_ASSERT(ctx);
	init_lexer(ctx->parser_.lexer);
	token tk = lex_token(ctx->parser_lexer);
	while (tk != TK_EOF)
	{
		tk = lex_token(ctx->parser_lexer);
	}
}
