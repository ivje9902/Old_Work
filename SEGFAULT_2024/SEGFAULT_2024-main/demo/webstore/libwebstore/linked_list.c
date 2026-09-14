#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "linked_list.h"
#include "private.h"
#include "../../../src/refmem.h"

// To address a current bug with the refmem implementation as it
// doesnt check if the passed destructor function is a null pointer
static void dummy_destructor(__attribute_maybe_unused__ obj *o) {}

static void linked_list_destructor(obj *o) {
    ioopm_list_t *list = (ioopm_list_t *) o;
    ioopm_linked_list_clear_demo(list);
    release(list->first);
}

static link_t *link_new(elem_t value, link_t *next)
{
    link_t *new_link = allocate(sizeof(link_t), dummy_destructor);
    if (!new_link) {
        return NULL;
    }
    retain(new_link);

    new_link->element = value;
    new_link->next = next;
    return new_link;
}

static elem_t link_remove(link_t *link)
{
    link_t *to_remove = link->next;
    elem_t value = to_remove->element;
    link->next = to_remove->next;
    release(to_remove);
    return value;
}

ioopm_list_t *ioopm_linked_list_create_demo(ioopm_eq_function eq_func)
{
    ioopm_list_t *result = allocate(sizeof(ioopm_list_t), linked_list_destructor);
    if (!result)
    {
        return NULL;
    }
    retain(result);

    result->first = allocate(sizeof(link_t), dummy_destructor);
    if (!result->first) {
        return NULL;
    }
    retain(result->first);
    result->first->next = NULL;

    result->last = result->first;
    result->size = 0;
    result->eq_func = eq_func;

    return result;
}

void ioopm_linked_list_clear_demo(ioopm_list_t *list)
{
    while (ioopm_linked_list_size_demo(list) > 0)
    {
        ioopm_linked_list_remove_demo(list, 0);
    }
}

void ioopm_linked_list_destroy_demo(ioopm_list_t *list)
{
    release(list);
}

static int linked_list_inner_adjust_index(int index, int upper_bound)
{
    int new_index;
    if (index < 0)
    {
        new_index = 0;
    }
    else if (index >= upper_bound)
    {
        new_index = upper_bound;
    }
    else
    {
        new_index = index;
    }
    return new_index;
}

static link_t *linked_list_find_previous(link_t *link, int index)
{
    link_t *cursor = link;
    for (int i = 0; i < index; i++)
    {
        cursor = cursor->next;
    }
    return cursor;
}

void ioopm_linked_list_insert_demo(ioopm_list_t *list, int index, elem_t value)
{
    int valid_index = linked_list_inner_adjust_index(index, ioopm_linked_list_size_demo(list) + 1);
    link_t *prev = linked_list_find_previous(list->first, valid_index);
    prev->next = link_new(value, prev->next);
    list->size++;
}

void ioopm_linked_list_append_demo(ioopm_list_t *list, elem_t value)
{
    /// Insert element at the end of the list
    link_t *link = link_new(value, NULL);
    list->last->next = link;
    /// Update the last pointer to point to the new last element
    list->last = link;
    list->size++;
}

void ioopm_linked_list_prepend_demo(ioopm_list_t *list, elem_t value)
{
    ioopm_linked_list_insert_demo(list, 0, value);
}

size_t ioopm_linked_list_size_demo(ioopm_list_t *list)
{
    return list->size;
}

elem_t ioopm_linked_list_remove_demo(ioopm_list_t *list, int index)
{
    int valid_index = linked_list_inner_adjust_index(index, ioopm_linked_list_size_demo(list));
    link_t *prev = linked_list_find_previous(list->first, valid_index);
    list->size--;
    if (list->size == 0)
    {
        list->last = list->first;
    }
    return link_remove(prev);
}

elem_t *ioopm_linked_list_get_demo(ioopm_list_t *list, int index)
{
    int valid_index = linked_list_inner_adjust_index(index, ioopm_linked_list_size_demo(list));

    link_t *cursor = linked_list_find_previous(list->first, valid_index);

    return &cursor->next->element;
}

bool ioopm_linked_list_is_empty_demo(ioopm_list_t *list)
{
    return (list->size == 0);
}

bool ioopm_linked_list_all_demo(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    size_t size = ioopm_linked_list_size_demo(list);
    bool result = true;

    for (int i = 0; i < (int) size && result; i++)
    {
        elem_t current = *ioopm_linked_list_get_demo(list, i);
        result = result && prop(current, current, extra);
    }
    return result;
}

bool ioopm_linked_list_any_demo(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    size_t size = ioopm_linked_list_size_demo(list);
    bool result = false;

    for (int i = 0; i < (int) size && !result; i++)
    {
        elem_t current = *ioopm_linked_list_get_demo(list, i);
        result = prop(current, current, extra);
    }
    return result;
}

bool ioopm_linked_list_contains_demo(ioopm_list_t *list, elem_t element)
{
    for (link_t *cursor = list->first->next; cursor; cursor = cursor->next)
    {
        if (list->eq_func(cursor->element, element))
        {
            return true;
        }
    }

    return false;
}

void ioopm_linked_list_apply_to_all_demo(ioopm_list_t *list, ioopm_apply_function fun, void *extra)
{
    size_t size = ioopm_linked_list_size_demo(list);
    link_t *current = list->first->next;

    for (int i = 0; i < (int) size; i++)
    {
        fun(current->element, &current->element, extra);
        current = current->next;
    }
}
