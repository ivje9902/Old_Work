#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "private.h"
#include "../../../src/refmem.h"

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
    ioopm_hash_function hash_f; // Possible NULL
    size_t size;
};

// To address a current bug with the refmem implementation as it
// doesnt check if the passed destructor function is a null pointer
static void dummy_destructor(__attribute_maybe_unused__ obj *o) {}

// Since the hash table doesnt take inte any destructor function we can assume that the allocation of an object does not need
ioopm_hash_table_t *ioopm_hash_table_create_demo(ioopm_hash_function hash_f, ioopm_eq_function key_f, ioopm_eq_function value_f)
{
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
    /// entry_t's, which will be set to NULL
    ioopm_hash_table_t *result = allocate(sizeof(ioopm_hash_table_t), (function1_t) ioopm_hash_table_clear_demo);

    // Tell the refmem library that we are using a reference to the object.
    retain(result);

    // Since refmem allocate function is the equivalent of malloc()
    // we now also need to initialize our buckets array manually,
    // otherwise we will get "Uninitialised value was created by a
    // heap allocation" from Valgrind. The other members below are
    // explicitly initialized below the following line.
    memset(&result->buckets, 0, sizeof(struct hash_table));

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

void ioopm_hash_table_destroy_demo(ioopm_hash_table_t *ht)
{
    // Release this librarys reference to the hash-table object
    release(ht);
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
    // We dont have to do any memset after this allocation as
    // all members of he entry object are explicitly initialized.
    entry_t *new_entry = allocate(sizeof(entry_t), dummy_destructor);

    // Tell the refmem library that this hash table library will
    // keep a internal reference of this object
    retain(new_entry);

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
    release(tmp);
    return value;
}

void ioopm_hash_table_insert_demo(ioopm_hash_table_t *ht, elem_t key, elem_t value)
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

option_t ioopm_hash_table_lookup_demo(ioopm_hash_table_t *ht, elem_t key)
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

option_t ioopm_hash_table_remove_demo(ioopm_hash_table_t *ht, elem_t key)
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

size_t ioopm_hash_table_size_demo(ioopm_hash_table_t *ht)
{
    return ht->size;
}

bool ioopm_hash_table_is_empty_demo(ioopm_hash_table_t *ht)
{
    return ioopm_hash_table_size_demo(ht) == 0;
}

void ioopm_hash_table_clear_demo(ioopm_hash_table_t *ht)
{
    if (ioopm_hash_table_is_empty_demo(ht))
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

ioopm_list_t *ioopm_hash_table_keys_demo(ioopm_hash_table_t *ht)
{
    ioopm_list_t *keys = ioopm_linked_list_create_demo(ht->eq_func_key);

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = &ht->buckets[i];
        while (current->next)
        {
            current = current->next;
            ioopm_linked_list_append_demo(keys, current->key);
        }
    }
    return keys;
}

ioopm_list_t *ioopm_hash_table_values_demo(ioopm_hash_table_t *ht)
{
    ioopm_list_t *values = ioopm_linked_list_create_demo(ht->eq_func_value);

    for (int i = 0; i < No_Buckets; i++)
    {
        entry_t *current = &ht->buckets[i];
        while (current->next)
        {
            current = current->next;
            ioopm_linked_list_append_demo(values, current->value);
        }
    }
    return values;
}

bool ioopm_hash_table_has_key_demo(ioopm_hash_table_t *ht, elem_t key)
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

bool ioopm_hash_table_has_value_demo(ioopm_hash_table_t *ht, elem_t value)
{
    ioopm_list_t *values = ioopm_hash_table_values_demo(ht);
    bool contains = ioopm_linked_list_contains_demo(values, value);
    ioopm_linked_list_destroy_demo(values);
    return contains;
}

bool ioopm_hash_table_all_demo(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size_demo(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys_demo(ht);
    ioopm_list_iterator_t *iter_keys = ioopm_list_iterator_demo(keys);
    ioopm_list_t *values = ioopm_hash_table_values_demo(ht);
    ioopm_list_iterator_t *iter_values = ioopm_list_iterator_demo(values);

    bool result = true;
    for (int i = 0; i < (int) size && result; ++i)
    {
        result = result && pred(ioopm_iterator_next_demo(iter_keys), ioopm_iterator_next_demo(iter_values), arg);
    }
    ioopm_linked_list_destroy_demo(keys);
    ioopm_iterator_destroy_demo(iter_keys);
    ioopm_linked_list_destroy_demo(values);
    ioopm_iterator_destroy_demo(iter_values);
    return result;
}

bool ioopm_hash_table_any_demo(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size_demo(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys_demo(ht);
    ioopm_list_iterator_t *iter_keys = ioopm_list_iterator_demo(keys);
    ioopm_list_t *values = ioopm_hash_table_values_demo(ht);
    ioopm_list_iterator_t *iter_values = ioopm_list_iterator_demo(values);

    bool result = false;
    for (int i = 0; i < (int) size && !result; ++i)
    {
        result = pred(ioopm_iterator_next_demo(iter_keys), ioopm_iterator_next_demo(iter_values), arg);
    }
    ioopm_linked_list_destroy_demo(keys);
    ioopm_iterator_destroy_demo(iter_keys);
    ioopm_linked_list_destroy_demo(values);
    ioopm_iterator_destroy_demo(iter_values);
    return result;
}

void ioopm_hash_table_apply_to_all_demo(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
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
