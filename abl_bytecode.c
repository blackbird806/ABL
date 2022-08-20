#include "abl_bytecode.h"

#include "abl_value.h"
#include <stdbool.h>

static void grow_chunk(bytecode_chunk* c)
{
	c->capacity = c->capacity < 8 ? 8 : c->capacity * 2;
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

static int disassemble_constant(bytecode_chunk* c, FILE* out, int offset)
{
	uint32_t const index = *(uint32_t*)&c->code[offset];
	offset += sizeof(uint32_t);
	value_type const type = (value_type)c->code[offset];
	offset++;
	fprintf(out, "[%d] ", index);
	switch (type)
	{
	case VAL_BOOL:
		*(abl_bool*)&c->code[offset] == true ? fprintf(out, " BOOL: TRUE") : fprintf(out, "FALSE");
		offset += sizeof(abl_bool);
		break;
	case VAL_INT:
		fprintf(out, "INT: %d\n", *(abl_int*)&c->code[offset]);
		offset += sizeof(abl_int);
		break;
	case VAL_FLOAT:
		fprintf(out, "FLOAT %f\n", *(abl_float*)&c->code[offset]);
		offset += sizeof(abl_float);
		break;
	default: 
		fprintf(out, "type not recognized\n");
	}
	return offset;
}

void disassemble_chunk(bytecode_chunk* c, FILE* out)
{
	ABL_ASSERT(c);
	ABL_ASSERT(out);
	int off = 0;
	fprintf(out, ".CODE SECTION\n");
	while (off < c->size)
	{
		off = disassemble_instruction(c, out, off);
		if (*(section_code*)&c->code[off] == SECTION_CONSTANTS)
			break;
	}
	off++;
	fprintf(out, ".CONSTANTS SECTION\n");
	while (off < c->size)
	{
		off = disassemble_constant(c, out, off);
	}
}

static int simple_instruction(const char* c, FILE* out, int offset)
{
	fprintf(out, "%s\n", c);
	return offset + 1;
}

static int constant_instruction(bytecode_chunk* chunk, FILE* out, int offset)
{
	fprintf(out, "CONST ");
	offset++;
	fprintf(out, "%d\n", *(uint32_t*)&chunk->code[offset]);
	offset += sizeof(uint32_t);
	return offset;
}

int disassemble_instruction(bytecode_chunk* c, FILE* out, int offset)
{
	#define SIMPLE_INSTRUCTION(name) case OP_##name : return simple_instruction(#name, out, offset)

	fprintf(out, "%04d ", offset);
	uint8_t const instruction = c->code[offset];
	switch (instruction)
	{
		SIMPLE_INSTRUCTION(NOP);
		SIMPLE_INSTRUCTION(ADD);
		SIMPLE_INSTRUCTION(SUB);
		SIMPLE_INSTRUCTION(RET);
		SIMPLE_INSTRUCTION(MUL);
		SIMPLE_INSTRUCTION(DIV);
		SIMPLE_INSTRUCTION(PUSH);
		SIMPLE_INSTRUCTION(POP);
		case OP_CONST:
			return constant_instruction(c, out, offset);
	}
}


