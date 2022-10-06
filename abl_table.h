#ifndef ABL_TABLE_H
#define ABL_TABLE_H

#include "abl_core.h"
#include "abl_value.h"

typedef struct {
	//ObjString* key;
	abl_value value;
} abl_table_entry;

typedef struct
{
	int size;
	int capacity;
	abl_table_entry* entry;
} abl_table;

#endif
