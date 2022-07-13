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

void disassemble_chunk(bytecode_chunk* c, FILE* out)
{
	ABL_ASSERT(c);
	ABL_ASSERT(out);
	
	for (int off = 0; off < chunk->size;)
	{
		off = disassemble_instruction(c, out, off);
	}
}

static int simple_instruction(const char* c, FILE* out, int offset)
{
	fprintf(out, "%s\n", c);
	return offset + 1;
}

const char* disassemble_instruction(bytecode_chunk* c, FILE* out, int offset)
{
	fprintf(out, "%04d ", offset);
	uint8_t instruction = chunk->code[offset];
	switch (instruction)
	{
		case OP_NOP: return simple_instruction("NOP", offset);
		case OP_RETURN: return simple_instruction("RETURN", offset);
		case OP_CONSTANT: 
	}
}
