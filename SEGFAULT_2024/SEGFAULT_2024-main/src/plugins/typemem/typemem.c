#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../lib/libioopm/hash_table.h"
#include "../../lib/libmeta/meta.h"
#include <stdarg.h>
#include <stddef.h>
#include "typemem.h"

/**
 * @file typemem.c
 * @brief Implementation of a type registry with memory operations.
 */

// Global hash table to register data types
static ioopm_hash_table_t *type_register;

/**
 * @brief Equality function for the hash table.
 * @param e1 The first element to compare.
 * @param e2 The second element to compare.
 * @return True if the elements are equal, false otherwise.
 */
static bool string_eq(elem_t e1, elem_t e2)
{
    return (strcmp(e1.pointer_value, e2.pointer_value) == 0);
}

/**
 * @brief Hash function for string keys in the hash table.
 * @param e The element to hash.
 * @return The hash value.
 */
static int string_sum_hash(elem_t e)
{
    char *str = e.pointer_value;
    int result = 0;
    do
    {
        result += *str;
    } while (*++str != '\0');
    return result;
}

/**
 * @brief Constructor function that initializes the hash table at load.
 */
static void __attribute__((constructor)) init()
{
    type_register = ioopm_hash_table_create(string_sum_hash, string_eq, NULL);
    return;
}

/**
 * @brief Function to read data from the hash table.
 * @param type The type to look up in the registry.
 * @return A list of offsets associated with the given type.
 */
ioopm_list_t *__read_mem(char *type)
{
    option_t register_lookup = ioopm_hash_table_lookup(type_register, (elem_t){.pointer_value = type});
    if (register_lookup.success)
    {
        // TODO maybe add error handling for rewriting data type
        ioopm_list_t *offsets = register_lookup.value.pointer_value;
        return offsets;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief Function to write data to the hash table.
 * @param dest The destination list to append data.
 * @param src The source data to be written.
 * @param size The size of the source data.
 */
void write_mem(ioopm_list_t *dest, size_t *src, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        ioopm_linked_list_append(dest, (elem_t){.ul_t = src[i]});
    }
}

/**
 * @brief Function to read the size of a data type from the hash table.
 * @param type The type to get the size for.
 * @return The size of the specified type.
 */
size_t __read_bytes(char *type)
{
    ioopm_list_t *mem = __read_mem(type);
    if (mem)
    {
        return ioopm_linked_list_get(mem, 0)->ul_t;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief Function to insert offsets for a new data type into the hash table.
 * @param type The type to register.
 * @param offsets An array of offsets.
 * @param size The size of the offsets array.
 */
void insert_offsets(char *type, size_t *offsets, size_t size)
{
    if (__read_mem(type) != NULL)
    {
        // Error: trying to redefine type
        return;
    }
    else
    {
        ioopm_list_t *buf = ioopm_linked_list_create(NULL);
        write_mem(buf, offsets, size);
        ioopm_hash_table_insert(type_register, (elem_t){.string_value = type}, (elem_t){.pointer_value = buf});
    }
}

/**
 * @brief Function to register a new data type with its size and offsets.
 * @param type The type to register.
 * @param size The size of the data type.
 * @param num_offsets The number of offsets to follow.
 * @param ... Variable number of offsets.
 */
void __register_type(char *type, size_t size, int num_offsets, ...)
{
    size_t offsets[num_offsets + 1]; // [size, offset1, offset2, ...]
    offsets[0] = size;               // Store size information at the beginning

    va_list args;
    va_start(args, num_offsets);

    for (int i = 1; i <= num_offsets; i++)
    {
        offsets[i] = va_arg(args, size_t);
    }

    va_end(args);

    // Pass the total number of elements (size + offsets)
    insert_offsets(type, offsets, num_offsets + 1);
}

/**
 * @brief Function to apply to all values in the hash table during destruction.
 * @param key The key associated with the current value.
 * @param value The value to be freed.
 * @param arg Additional argument (unused).
 */
void free_register(elem_t key, elem_t *value, void *arg)
{
    ioopm_linked_list_destroy(value->pointer_value);
}

/**
 * @brief Function to free all value pointers and destroy the hash table.
 */
void destroy_register()
{
    ioopm_hash_table_apply_to_all(type_register, free_register, 0);
    ioopm_hash_table_destroy(type_register);
}

/**
 * @brief Destructor function to free all resources during program exit.
 */
static void __attribute__((destructor)) done()
{
    destroy_register();
}

/**
 * @brief Default function to release resources for an object.
 * @param o The object to release.
 */
void __default(obj *o)
{
    meta_data *object = obj_to_meta(o);
    ioopm_list_t *offsets = __read_mem(object->type);
    size_t size = ioopm_linked_list_size(offsets);
    size_t *to_release;
    for (int i = 1; i < size; i++)
    {
        size_t offset = ioopm_linked_list_get(offsets, i)->ul_t;
        to_release = (size_t *)((char *)o + offset);
        release((obj *)*to_release);
    }
}

/**
 * @brief Function to allocate memory for an object of a specified type.
 * @param type The type of object to allocate.
 * @return A pointer to the allocated object.
 */
obj *__allocate_from_type(char *type)
{
    size_t type_size = __read_bytes(type);
    size_t block_size = sizeof(meta_data) + type_size;
    meta_data *object = calloc(1, block_size);
    if (!object)
    {
        return object;
    }

    object->ref_count = 0;
    object->type = type;
    object->destroy = __default;

    return meta_to_obj(object);
}