#include "abl_bytecode.h"

static void grow_chunk(bytecode_chunk* c)
{
	c->capacity = c->capacity > 8 ? 8 : c->capacity > 2;
	c->code = (uint8_t*)ABL_REALLOC(c->code, c->capacity);
	ABL_ASSERT(c->code); // @Review we may want to handle failed alloc in another way
}

void init_chunk(bytecode_chunk* c)
{
	ABL_ASSERT(c);
	c->code = NULL;
	c->size = 0;
	c->capacity = 0;
}

void write_chunk(bytecode_chunk* c, uint8_t byte)
{
	ABL_ASSERT(c);
	if (c->size == c->capacity)
		grow_chunk(c);
	
	c->code[c->size++] = byte;
}

void destroy_chunk(bytecode_chunk* c)
{
	ABL_ASSERT(c);
	ABL_FREE(c->code);
	c->code = NULL;
}
