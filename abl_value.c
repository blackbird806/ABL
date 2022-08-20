#include "abl_value.h"

abl_value make_int(abl_int val)
{
	return (abl_value) { VAL_INT, { .i = val } };
}

abl_value make_float(abl_float val)
{
	return (abl_value) { VAL_FLOAT, { .f = val } };
}

abl_value make_bool(bool val)
{
	return (abl_value) { VAL_BOOL, { .b = val } };
}

void abl_value_array_init(abl_value_array* arr)
{
	ABL_ASSERT(arr);
	arr->values = NULL;
	arr->size = 0;
	arr->capacity = 0;
}

static void grow_value_array(abl_value_array* arr)
{
	arr->capacity = arr->capacity < 8 ? 8 : arr->capacity * 2;
	arr->values = ABL_REALLOC(arr->values, arr->capacity);
	ABL_ASSERT(arr->values); // @Review we may want to handle failed alloc in another way
}

void abl_value_array_add(abl_value_array* arr, abl_value val)
{
	ABL_ASSERT(arr);
	if (arr->size == arr->capacity)
		grow_value_array(arr);
	arr->values[arr->size++] = val;
}

void abl_value_array_destroy(abl_value_array* arr)
{
	ABL_ASSERT(arr);
	ABL_FREE(arr->values);
	arr->size = 0;
	arr->capacity = 0;
}


