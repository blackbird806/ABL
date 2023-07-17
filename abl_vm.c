#include "abl_vm.h"

void abl_vm_init(abl_vm* vm)
{
	ABL_ASSERT(vm);
	vm->stack_top = vm->stack;
	vm->pc = NULL;

	abl_table_init(&vm->strings);

	abl_value_array_init(&vm->constants);
	abl_table_init(&vm->global_variables);
}

void abl_vm_get_constants_from_compiler(abl_vm* vm, abl_compiler* compiler)
{
	ABL_ASSERT(vm);
	ABL_ASSERT(compiler);
	vm->constants = abl_value_array_move(&compiler->constants);
}

void abl_vm_destroy(abl_vm* vm)
{
	ABL_ASSERT(vm);

	abl_table_destroy(&vm->strings);
}

static void push(abl_vm* vm, abl_value value)
{
	*vm->stack_top = value;
	vm->stack_top++;
}

static abl_value pop(abl_vm* vm)
{
	return *(--vm->stack_top);
}

static uint32_t read32(abl_vm* vm)
{
	uint32_t const val = *vm->pc;
	vm->pc += sizeof(uint32_t);
	return val;
}


// TODO
static void read_constants(abl_vm* vm, bytecode_chunk* c)
{
	int off = 0;

	if ((uint8_t)c->code[off++] != SECTION_CONSTANTS)
		return;

	uint32_t const size = (uint32_t)c->code[off];
	off += sizeof(uint32_t);

	for (uint32_t i = 0; i < size; i++)
	{

	}
}

#define BIN_OP(OP) {abl_value a = pop(vm), b = pop(vm); if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) { push(vm, make_float(a.v.f OP b.v.f)); } else { push(vm, make_int(a.v.i OP b.v.i)); }}
#define BIN_OPR(OP) {abl_value a = pop(vm), b = pop(vm); if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) { push(vm, make_float(b.v.f OP a.v.f)); } else { push(vm, make_int(b.v.i OP a.v.i)); }}

abl_interpret_result abl_vm_interpret(abl_vm* vm, bytecode_chunk* chunk)
{
	ABL_ASSERT(vm);
	ABL_ASSERT(chunk);

	vm->pc = ++chunk->code; // skip section name
	while (vm->pc != chunk->code + chunk->size)
	{
		uint8_t const instruction = *vm->pc++;
		switch (instruction)
		{
		case OP_NOP:
			continue;
		case OP_VARDECL:
		{
			uint32_t const const_id = read32(vm);
			abl_string* key = (abl_string*)vm->constants.values[const_id].v.o;
			if (!abl_table_set(&vm->global_variables, key, make_null()))
			{
				ABL_DEBUG_DIAGNOSTIC("var '%.*s' already declared", key->size, key->data);
			}
			break;
		}
		case OP_STORE:
		{
			uint32_t const const_id = read32(vm);
			abl_string* key = (abl_string*)vm->constants.values[const_id].v.o;
			abl_table_set(&vm->global_variables, key, pop(vm));
			break;
		}
		case OP_LOAD:
		{
			abl_string const* key = (abl_string*)pop(vm).v.o;
			abl_value val = make_null();
			abl_table_get(&vm->global_variables, key, &val);
			push(vm, val);
			break;
		}
		case OP_STORE_LOCAL:
		{
			uint32_t const slot = read32(vm);
			vm->stack[slot] = vm->stack[0];
			break;
		}
		case OP_LOAD_LOCAL:
		{
			uint32_t const slot = read32(vm);
			push(vm, vm->stack[slot]);
			break;
		}
		case OP_PUSHC:
		{
			uint32_t const_id = read32(vm);
			push(vm, vm->constants.values[const_id]);
			break;
		}
		case OP_POP:
			pop(vm); // discard value
			break;
		case OP_ADD:
			BIN_OP(+)
			break;
		case OP_SUB:
			BIN_OPR(-)
			break;
		case OP_MUL:
			BIN_OP(*)
			break;
		case OP_DIV:
			BIN_OPR(/)
			break;
		case OP_NEG:
			if (vm->stack_top->type == VAL_INT) vm->stack_top->v.i = -vm->stack_top->v.i;
			else vm->stack_top->v.f = -vm->stack_top->v.f;
			break;
		}

	}
}
