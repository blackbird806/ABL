#ifdnef ABL_BYTECODE_H
#define ABL_BYTECODE_H

#include "abl_core.h"

typedef enum {
	
	OP_NOP,
	OP_ADD, OP_SUB, OP_DIV, OP_MUL,
	OP_CONST,
	OP_RETURN
} op_code;

typedef struct {
	uint8_t* code;
	int size;
	int capacity;
} bytecode_chunk;

void init_chunk(bytecode_chunk* c);
void write_chunk(bytecode_chunk* c, uint8_t byte);
void destroy_chunk(bytecode_chunk* c);

#endif
