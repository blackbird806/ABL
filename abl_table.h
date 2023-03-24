#ifndef ABL_TABLE_H
#define ABL_TABLE_H

#include "abl_core.h"
#include "abl_value.h"

typedef struct {
	abl_string* key;
	abl_value value;
} abl_table_entry;

typedef struct
{
	uint32_t size;
	uint32_t capacity;
	abl_table_entry* entries;
} abl_table;

void abl_table_init(abl_table* table);
void abl_table_destroy(abl_table* table);
bool abl_table_set(abl_table* table, abl_string* key, abl_value value);
bool abl_table_get(abl_table const* table, abl_string const* key, abl_value* value);
abl_string* abl_table_find_string(abl_table* table, const abl_char* chars, uint32_t size, uint32_t hash);
bool abl_table_delete(abl_table* table, abl_string const* key);
void abl_table_copy(abl_table const* from, abl_table* to);

#endif
