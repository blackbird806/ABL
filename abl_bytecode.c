#include "abl_bytecode.h"

#include "abl_value.h"
#include <stdbool.h>
#include <string.h>

// @Bug realloc seems to fail sometimes
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

void writestr_chunk(bytecode_chunk* c, abl_string* str)
{
	ABL_ASSERT(c);
	ABL_ASSERT(str);

	int const strmemsize = sizeof(abl_char) * str->size;
	if (c->size + strmemsize > c->capacity)
		grow_chunk(c);
	memcpy(&c->code[c->size], str->data, strmemsize);
	c->size += strmemsize;
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
	value_type const type = (uint8_t)c->code[offset];
	offset++;
	fprintf(out, "[%d] ", index);
	switch (type)
	{
	case VAL_BOOL:
		*(abl_bool*)&c->code[offset] == true ? fprintf(out, " BOOL: TRUE") : fprintf(out, "BOOL: FALSE");
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
	case VAL_OBJ:
		{
			object_type const obj_type = c->code[offset];
			offset++;
			switch (obj_type)
			{
			case OBJ_STRING:
				{
					uint32_t const str_len = *(uint32_t*)&c->code[offset];
					offset += sizeof(uint32_t);
					int const strsize = str_len * sizeof(abl_char);
					abl_char* str = (abl_char*)&c->code[offset];
					printf("\"%.*s\"\n", str_len, str);
					offset += strsize;
				}
				break;
			}
		}
		break;
	default: 
		fprintf(out, "type not recognized\n");
	}
	return offset;
}

void disassemble_chunk(bytecode_chunk* c, FILE* out)
{
	ABL_ASSERT(c);
	ABL_ASSERT(c->size > 0);
	ABL_ASSERT(out);
	int off = 0;
	section_code section = (uint8_t)c->code[off++];
	if (section == SECTION_CONSTANTS)
	{
		fprintf(out, ".CONSTANTS SECTION\n");
		uint32_t const nb_constants = (uint32_t)c->code[off];
		off += sizeof(uint32_t);
		fprintf(out, "%d constants\n", nb_constants);
		while (off < c->size)
		{
			off = disassemble_constant(c, out, off);
		}
	}
	if (section == SECTION_CODE)
	{
		fprintf(out, ".CODE SECTION\n");
		while (off < c->size)
		{
			off = disassemble_instruction(c, out, off);
			if (*(section_code*)&c->code[off] == SECTION_CONSTANTS)
				break;
		}
	}
}

static int simple_instruction(const char* c, FILE* out, int offset)
{
	fprintf(out, "%s\n", c);
	return offset + 1;
}

static int constant_instruction(bytecode_chunk* chunk, FILE* out, int offset)
{
	fprintf(out, "PUSHC ");
	offset++;
	fprintf(out, "%d\n", *(uint32_t*)&chunk->code[offset]);
	offset += sizeof(uint32_t);
	return offset;
}

int disassemble_instruction(bytecode_chunk* c, FILE* out, int offset)
{
	#define SIMPLE_INSTRUCTION(name) case OP_##name : return simple_instruction(#name, out, offset)

	fprintf(out, "%04X ", offset);
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
		case OP_PUSHC:
			return constant_instruction(c, out, offset);
	}
}

int move_next_instruction(bytecode_chunk* c, int offset)
{
	uint8_t const instruction = c->code[offset];
	switch (instruction)
	{
		case OP_PUSHC:
			return sizeof(uint8_t) + sizeof(uint32_t);
		default:
			return sizeof(uint8_t);
	}
}


