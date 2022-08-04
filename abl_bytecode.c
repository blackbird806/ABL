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

void write4_chunk(bytecode_chunk* c, uint32_t bytes)
{
	ABL_ASSERT(c);
	if (c->size + sizeof(bytes) > c->capacity)
		grow_chunk(c);
	
	*(uint32_t*)(&c->code[c->size]) = bytes;
	c->size += sizeof(bytes);
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

static int constant_instruction(bytecode_chunk* chunk, FILE* out, int offset)
{
	fprintf(out, "CONSTANT ");
	value_type const type = chunk->code[++offset];
	switch(type)
	{
		case VAL_BOOL: 
			fprintf(out, "BOOL %s\n", chunk->code[offset] == 0 ? "FALSE" : "TRUE");
			offset++;
			break;
		case VAL_INT:
			// fprintf(out, "INT %d\n", chunk->);
	}
	return offset;
}

int disassemble_instruction(bytecode_chunk* c, FILE* out, int offset)
{
	#define SIMPLE_INSTRUCTION(name) case OP_##name : return simple_instruction(#name, out, offset)

	fprintf(out, "%04d ", offset);
	uint8_t instruction = c->code[offset];
	switch (instruction)
	{
		SIMPLE_INSTRUCTION(NOP);
		SIMPLE_INSTRUCTION(ADD);
		SIMPLE_INSTRUCTION(SUB);
		SIMPLE_INSTRUCTION(RET);
		SIMPLE_INSTRUCTION(MUL);
		SIMPLE_INSTRUCTION(DIV);
		case OP_CONST:
	}
}


