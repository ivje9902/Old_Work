#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "linked_list.h"

#define Success(v) (option_t){.success = true, .value = v};
#define Failure() (option_t){.success = false};
#define Successful(o) (o.success == true)
#define Unsuccessful(o) (o.success == false)

/**
 * @file hash_table.h
 * @author Elias Benjaminsson, Jesper Borglund Vannebäck
 * @date  11 Sep 2023
 * @brief Simple hash table that maps integer keys to string values.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gb54499f3b7b264e3af3b68c756090f52
 */

typedef struct hash_table ioopm_hash_table_t;
typedef struct option option_t;
typedef int (*ioopm_hash_function)(elem_t element);

struct option
{
    bool success;
    elem_t value;
};

/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create_demo(ioopm_hash_function hash_f, ioopm_eq_function key_f, ioopm_eq_function value_f);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy_demo(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert_demo(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return an option struct where if succesful, the value and a true bool are returned,
/// if unsuccesful, a false bool is returned.
option_t ioopm_hash_table_lookup_demo(ioopm_hash_table_t *ht, elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return an option struct where if succesful, the value to be removed and a true bool are returned,
/// if unsuccesful, a false bool is returned.
option_t ioopm_hash_table_remove_demo(ioopm_hash_table_t *ht, elem_t key);

/// @brief returns the number of key => value entries in the hash table
/// @param ht hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size_demo(ioopm_hash_table_t *ht);

/// @brief checks if the hash table is empty
/// @param ht hash table operated upon
/// @return true is size == 0, else false
bool ioopm_hash_table_is_empty_demo(ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param ht hash table operated upon
void ioopm_hash_table_clear_demo(ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param ht hash table operated upon
/// @return a linked list of keys for hash table ht
ioopm_list_t *ioopm_hash_table_keys_demo(ioopm_hash_table_t *ht);

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param ht hash table operated upon
/// @return a linked list of values for hash table ht
ioopm_list_t *ioopm_hash_table_values_demo(ioopm_hash_table_t *ht);

/// @brief check if a hash table has an entry with a given key
/// @param ht hash table operated upon
/// @param key the key sought
/// @return true if key exists, false if not
bool ioopm_hash_table_has_key_demo(ioopm_hash_table_t *ht, elem_t key);

/// @brief check if a hash table has an entry with a given value
/// @param ht hash table operated upon
/// @param value the value sought
/// @return true if value exists, false if not
bool ioopm_hash_table_has_value_demo(ioopm_hash_table_t *ht, elem_t value);

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if predicate is satisfied by all entries, false if not
bool ioopm_hash_table_all_demo(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if predicate is satisfied by any entry, false if not
bool ioopm_hash_table_any_demo(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief apply a function to all entries in a hash table
/// @param ht hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all_demo(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg);
