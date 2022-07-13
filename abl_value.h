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

#endif
