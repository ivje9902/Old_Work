#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "private.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define No_Buckets 17
#define Success(v) (option_t){.success = true, .value = v};
#define Failure() (option_t){.success = false};
#define Successful(o) (o.success == true)
#define Unsuccessful(o) (o.success == false)

typedef struct entry entry_t; // local type

struct entry
{
    elem_t key;    // holds the key
    elem_t value;  // holds the value
    entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
    entry_t buckets[No_Buckets];
    ioopm_eq_function eq_func_key;
    ioopm_eq_function eq_func_value;
    ioopm_hash_function hash_f; // Possivle NULL
    size_t size;
};

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_f, ioopm_eq_function key_f, ioopm_eq_function value_f)
{
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
    /// entry_t's, which will be set to NULL
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    result->eq_func_key = key_f;
    result->eq_func_value = value_f;
    result->hash_f = hash_f;
    result->size = 0;
    return result;
}

static int find_index(ioopm_hash_table_t *ht, elem_t key)
{
    int int_key;
    if (ht->hash_f == NULL)
    {
        // treat keys as integers
        int_key = key.int_value; // .i reads the integer part of the elem_t
    }
    else
    {
        int_key = ht->hash_f(key);
    }
    int_key = int_key % No_Buckets;
    return int_key;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    ioopm_hash_table_clear(ht);
    free(ht);
}

static entry_t *find_previous_entry_for_key(entry_t *bucket, elem_t key, ioopm_eq_function key_eq_func)
{
    entry_t *entry = bucket;
    while (entry->next && !key_eq_func(entry->next->key, key))
    {
        entry = entry->next;
    }
    return entry;
}

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next_entry)
{
    entry_t *new_entry = calloc(1, sizeof(entry_t));
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = next_entry;

    return new_entry;
}

static elem_t entry_remove(entry_t *entry)
{
    elem_t value = entry->next->value;
    entry_t *tmp = entry->next;
    entry->next = entry->next->next;
    free(tmp);
    return (value);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{

    /// Calculate the bucket for this entry
    int index = find_index(ht, key);
    if (index >= 0)
    {
        /// Search for an existing entry for a key
        entry_t *entry = find_previous_entry_for_key(&ht->buckets[index], key, ht->eq_func_key);
        entry_t *next = entry->next;
        /// Check if the next entry should be updated or not
        if (next != NULL && ht->eq_func_key(next->key, key))
        {
            next->value = value;
        }
        else
        {
            ht->size += 1;
            entry->next = entry_create(key, value, next);
        }
    }
}

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
    /// Calculate the bucket for this entry
    int index = find_index(ht, key);
    if (index >= 0)
    {
        /// Find the previous entry for key
        entry_t *entry = find_previous_entry_for_key(&ht->buckets[index], key, ht->eq_func_key);

        if (entry->next)
        {
            return Success(entry->next->value);
        }
        else
        {
            return Failure();
        }
    }
    else
    {
        puts("Not a valid key");
        return Failure();
    }
}

option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
    int index = find_index(ht, key);
    if (index >= 0)
    {
        /// Find the previous entry for key
        entry_t *entry = find_previous_entry_for_key(&ht->buckets[index], key, ht->eq_func_key);
        entry_t *remove = entry->next;

        if (remove)
        {
            ht->size -= 1;
            elem_t value = entry_remove(entry);
            return Success(value);
        }
        else
        {
            return Failure();
        }
    }
    else
    {
        puts("Not a valid key");
        return Failure();
    }
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
    return ht->size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
    return ioopm_hash_table_size(ht) == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
    if (ioopm_hash_table_is_empty(ht))
    {
        return;
    }
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = &ht->buckets[i];
        while (current->next != NULL)
        {
            entry_remove(current);
        }
    }
    ht->size = 0;
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    ioopm_list_t *keys = ioopm_linked_list_create(ht->eq_func_key);

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = &ht->buckets[i];
        while (current->next)
        {
            current = current->next;
            ioopm_linked_list_append(keys, current->key);
        }
    }
    return keys;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    ioopm_list_t *values = ioopm_linked_list_create(ht->eq_func_value);

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = &ht->buckets[i];
        while (current->next)
        {
            current = current->next;
            ioopm_linked_list_append(values, current->value);
        }
    }
    return values;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = &ht->buckets[i];
        while (current->next)
        {
            if (ht->eq_func_key(current->next->key, key))
            {
                return true;
            }
            current = current->next;
        }
    }
    return false;
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    bool contains = ioopm_linked_list_contains(values, value);
    ioopm_linked_list_destroy(values);
    return contains;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_iterator_t *iter_keys = ioopm_list_iterator(keys);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    ioopm_list_iterator_t *iter_values = ioopm_list_iterator(values);

    bool result = true;
    for (int i = 0; i < (int) size && result; ++i)
    {
        result = result && pred(ioopm_iterator_next(iter_keys), ioopm_iterator_next(iter_values), arg);
    }
    ioopm_linked_list_destroy(keys);
    ioopm_iterator_destroy(iter_keys);
    ioopm_linked_list_destroy(values);
    ioopm_iterator_destroy(iter_values);
    return result;
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_iterator_t *iter_keys = ioopm_list_iterator(keys);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    ioopm_list_iterator_t *iter_values = ioopm_list_iterator(values);

    bool result = false;
    for (int i = 0; i < (int) size && !result; ++i)
    {
        result = pred(ioopm_iterator_next(iter_keys), ioopm_iterator_next(iter_values), arg);
    }
    ioopm_linked_list_destroy(keys);
    ioopm_iterator_destroy(iter_keys);
    ioopm_linked_list_destroy(values);
    ioopm_iterator_destroy(iter_values);
    return result;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{
    {
        for (int i = 0; i < No_Buckets; i++)
        {
            entry_t *current = &ht->buckets[i];
            entry_t *next = current->next;
            while (next)
            {
                apply_fun(next->key, &next->value, arg);
                next = next->next;
            }
        }
    }
}
