#ifndef ABL_BYTECODE_H
#define ABL_BYTECODE_H

#include "abl_core.h"
#include "abl_value.h"

typedef enum {
	SECTION_CODE,
	SECTION_CONSTANTS
} section_code;

typedef enum {
	
	OP_NOP,
	OP_VARDECL,
	OP_STORE, OP_LOAD,
	OP_ADD, OP_SUB, OP_DIV, OP_MUL,
	OP_PUSHC,
	OP_NEG,
	OP_NOT,
	OP_PUSH, OP_POP,
	OP_RET
} op_code;

typedef struct {
	uint8_t* code;
	int size;
	int capacity;
} bytecode_chunk;

void init_chunk(bytecode_chunk* c);
void write_chunk(bytecode_chunk* c, uint8_t byte);
void write4_chunk(bytecode_chunk* c, uint32_t bytes);
void writestr_chunk(bytecode_chunk* c, abl_string* str);
void destroy_chunk(bytecode_chunk* c);

void disassemble_chunk(bytecode_chunk* c, FILE* outstream);
int disassemble_instruction(bytecode_chunk* c, FILE* out, int offset);
// rename this
int move_next_instruction(bytecode_chunk* c, int offset);

#endif
