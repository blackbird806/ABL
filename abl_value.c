#include "abl_value.h"
#include <string.h>

uint32_t hash_string(const char* str, int size)
{
	uint32_t hash = 2166136261u;
	for (int i = 0; i < size; i++) {
		hash ^= (uint8_t)str[i];
		hash *= 16777619;
	}
	return hash;
}

abl_value make_int(abl_int val)
{
	return (abl_value) { VAL_INT, { .i = val } };
}

abl_value make_null()
{
	return (abl_value) { VAL_NULL };
}

abl_value make_float(abl_float val)
{
	return (abl_value) { VAL_FLOAT, { .f = val } };
}

abl_value make_bool(bool val)
{
	return (abl_value) { VAL_BOOL, { .b = val } };
}

abl_value make_string(abl_string* str)
{
	return (abl_value) { VAL_OBJ, { .o = (abl_obj*)str } };
}

void abl_value_array_init(abl_value_array* arr)
{
	ABL_ASSERT(arr);
	arr->values = NULL;
	arr->size = 0;
	arr->capacity = 0;
}

abl_value_array abl_value_array_move(abl_value_array* arr)
{
	abl_value_array tmp = *arr;
	arr->capacity = 0;
	arr->size = 0;
	arr->values = NULL;
	return tmp;
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
