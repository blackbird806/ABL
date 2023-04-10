#include "abl_table.h"
#include <string.h> 

#define TABLE_MAX_LOAD 0.75

void abl_table_init(abl_table* table)
{
	ABL_ASSERT(table);
	table->size = 0;
	table->capacity = 0;
	table->entries = NULL;
}

void abl_table_destroy(abl_table* table)
{
	ABL_ASSERT(table);
	ABL_FREE(table->entries);
	abl_table_init(table); // nullify the struct
}

static abl_table_entry* find_entry(abl_table_entry* entries, uint32_t capacity, abl_string const* key)
{
	uint32_t index = key->hash % capacity;
	abl_table_entry* tombstone = NULL;
	while (true)
	{
		abl_table_entry* entry = &entries[index];
		if (entry->key == NULL)
		{
			if (entry->value.type == VAL_NULL)
			{
				return tombstone != NULL ? tombstone : entry;
			}
			// We found a tombstone.
			if (tombstone == NULL) 
				tombstone = entry;
		}
		else if (entry->key == key)
		{
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static void adjust_capacity(abl_table* table, uint32_t capacity)
{
	abl_table_entry* entries = ABL_MALLOC(capacity * sizeof(abl_table_entry));
	ABL_ASSERT(entries);
	for (uint32_t i = 0; i < capacity; i++)
	{
		entries[i].key = NULL;
		entries->value = make_null();
	}

	table->size = 0;
	for (uint32_t i = 0; i < table->capacity; i++)
	{
		abl_table_entry const* entry = &table->entries[i];
		if (entry->key == NULL)
			continue;

		abl_table_entry* dest = find_entry(entries, capacity, entry->key);
		ABL_FREE(table->entries);

		dest->key = entry->key;
		dest->value = entry->value;
		table->size++;
	}

	table->entries = entries;
	table->capacity = capacity;
}

bool abl_table_set(abl_table* table, abl_string* key, abl_value value)
{
	ABL_ASSERT(table);
	ABL_ASSERT(key);

	if (table->size + 1 > table->capacity * TABLE_MAX_LOAD)
	{
		uint32_t const new_capacity = table->capacity < 8 ? 8 : table->capacity * 2;
		adjust_capacity(table, new_capacity);
	}

	abl_table_entry* entry = find_entry(table->entries, table->capacity, key);
	bool const is_new_key = entry->key == NULL;

	if (is_new_key && entry->value.type == VAL_NULL)
		table->size++;

	entry->key = key;
	entry->value = value;
	return is_new_key;
}

void abl_table_copy(abl_table const* from, abl_table* to)
{
	ABL_ASSERT(from);
	ABL_ASSERT(to);

	for (uint32_t i = 0; i < from->capacity; i++)
	{
		abl_table_entry const* entry = &to->entries[i];
		if (entry->key != NULL)
		{
			abl_table_set(to, entry->key, entry->value);
		}
	}
}

bool abl_table_get(abl_table const* table, abl_string const* key, abl_value* value)
{
	ABL_ASSERT(table);
	ABL_ASSERT(key);
	ABL_ASSERT(value);

	if (table->size == 0)
		return false;

	abl_table_entry const* entry = find_entry(table->entries, table->capacity, key);
	if (entry->key == NULL)
		return false;

	*value =  entry->value;
	return true;
}

abl_string* abl_table_find_string(abl_table* table, const abl_char* chars, uint32_t size, uint32_t hash)
{
	if (table->size == 0)
		return NULL;

	uint32_t index = hash % table->capacity;
	while (true)
	{
		abl_table_entry* entry = &table->entries[index];
		if (entry->key == NULL)
		{
			if (entry->value.type == VAL_NULL)
				return NULL;
		} 
		else if (entry->key->size == size && entry->key->hash == hash &&
			memcmp(entry->key->data, chars, size) == 0)
		{
			return entry->key;
		}

		index = (index + 1) % table->capacity;
	}

}

bool abl_table_delete(abl_table* table, abl_string const* key)
{
	ABL_ASSERT(table);
	ABL_ASSERT(key);

	abl_table_entry* entry = find_entry(table->entries, table->capacity, key);
	if (entry->key == NULL)
		return false;

	// Place a tombstone in the entry.
	entry->key = NULL;
	entry->value = make_bool(true);
	return true;
}
