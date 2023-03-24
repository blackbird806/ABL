#ifndef ABL_VM_H
#define ABL_VM_H

#include "abl_bytecode.h"
#include "abl_compiler.h"
#include "abl_core.h"
#include "abl_value.h"
#include "abl_table.h"

#define ABL_STACK_MAX 256
#define ABL_CALLSTACK_MAX 128
#define ABL_MAX_CONSTANTS 256

typedef struct
{
	bytecode_chunk* code_chunk;
	abl_value_array constants;
	abl_table variables;
} abl_stack_frame;

typedef struct 
{
	abl_value stack[ABL_STACK_MAX];
	abl_value* stack_top;

	abl_stack_frame callstack[ABL_CALLSTACK_MAX];
	abl_stack_frame* current_frame;

	abl_table strings;

	uint8_t* pc;
} abl_vm;

typedef enum
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} abl_interpret_result;

void abl_vm_init(abl_vm* vm);
void abl_vm_get_constants_from_compiler(abl_vm* vm, abl_compiler* compiler);
void abl_vm_destroy(abl_vm* vm);
abl_interpret_result abl_vm_interpret(abl_vm* vm, bytecode_chunk* code);

#endif