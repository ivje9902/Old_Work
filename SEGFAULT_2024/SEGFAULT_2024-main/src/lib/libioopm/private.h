#pragma once
#include <stdlib.h>
#include <stdbool.h>

/// An element
typedef union elem elem_t;

union elem
{
    int int_value;
    unsigned int unsigned_value;
    bool bool_value;
    float float_value;
    char *string_value;
    void *pointer_value;
    size_t ul_t;
    /// other choices certainly possible
};
/// A link in the linked structure
typedef struct link link_t;

struct link
{
    elem_t element;
    link_t *next;
};

/// Compares two elements and returns true if they are equal
typedef bool (*ioopm_eq_function)(elem_t a, elem_t b);

typedef struct list ioopm_list_t; /// Meta: struct definition goes in C file

struct list
{
    link_t *first;
    link_t *last;
    size_t size;
    ioopm_eq_function eq_func;
};

typedef struct iter ioopm_list_iterator_t;

struct iter
{
    link_t *current;
    ioopm_list_t *list; /// New field
};

typedef bool (*ioopm_predicate)(elem_t key, elem_t element, void *extra);
typedef void (*ioopm_apply_function)(elem_t key, elem_t *element, void *extra);
