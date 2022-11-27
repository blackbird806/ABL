#ifndef ABL_VALUE_H
#define ABL_VALUE_H

#include "abl_core.h"

typedef enum {

	VAL_BOOL,
	VAL_INT,
	VAL_FLOAT,
	VAL_OBJ,
	VAL_NULL

} value_type;

typedef enum {

	OBJ_STRING,
	OBJ_TABLE,

} object_type;

typedef struct
{
	object_type type;
} abl_obj;

typedef struct
{
	abl_obj obj;
	uint32_t size;
	abl_char* data;
	uint32_t hash;
} abl_string;

typedef struct abl_value {
	value_type type;
	union { 
		abl_bool b;
		abl_float f;
		abl_int i;
		abl_obj* o;
	} v;
} abl_value;

abl_string allocate_string(char* chars, int size);
uint32_t hash_string(const char* str, int size);

abl_value make_int(abl_int val);
abl_value make_null();
abl_value make_float(abl_float val);
abl_value make_bool(bool val);
abl_value make_string(abl_string* val);

typedef struct abl_value_array {
	int capacity;
	int size;
	abl_value* values;
} abl_value_array;

void abl_value_array_init(abl_value_array* arr);
void abl_value_array_add(abl_value_array* arr, abl_value val);
void abl_value_array_destroy(abl_value_array* arr);

#endif
