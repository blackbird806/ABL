#ifndef ABL_VALUE_H
#define ABL_VALUE_H

#include "abl_core.h"

typedef enum {

	VAL_BOOL,
	VAL_INT,
	VAL_FLOAT

} value_type;

typedef struct abl_value {
	value_type type;
	union { 
		bool b;
		float f;
		int i;
	} v;
} abl_value;

typedef struct abl_value_array {
	int capacity;
	int size;
	abl_value* values;
} abl_value_array;

void abl_value_array_init(abl_value_array* arr);
void abl_value_array_add(abl_value_array* arr, abl_value val);
void abl_value_array_destroy(abl_value_array* arr);

#endif
