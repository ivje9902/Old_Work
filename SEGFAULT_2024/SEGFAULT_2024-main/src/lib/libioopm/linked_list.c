#include "linked_list.h"
#include "private.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static link_t *link_new(elem_t value, link_t *next)
{

    link_t *new_link = calloc(1, sizeof(link_t));

    if (new_link)
    {
        new_link->element = value;
        new_link->next = next;
    }

    return new_link;
}
static elem_t link_remove(link_t *link)
{
    link_t *to_remove = link->next;
    elem_t value = to_remove->element;
    link->next = to_remove->next;
    free(to_remove);
    return value;
}

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq_func)
{
    ioopm_list_t *result = calloc(1, sizeof(ioopm_list_t));

    if (result)
    {
        result->first = result->last = calloc(1, sizeof(link_t));
    }
    result->eq_func = eq_func;
    return result;
}

void ioopm_linked_list_clear(ioopm_list_t *list)
{
    while (ioopm_linked_list_size(list) > 0)
    {
        ioopm_linked_list_remove(list, 0);
    }
}

void ioopm_linked_list_destroy(ioopm_list_t *list)
{
    ioopm_linked_list_clear(list);
    free(list->first);
    free(list);
}

static int linked_list_inner_adjust_index(int index, int upper_bound)
{
    return (unsigned int)index % upper_bound;
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

void ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t value)
{
    int valid_index = linked_list_inner_adjust_index(index, ioopm_linked_list_size(list) + 1);
    link_t *prev = linked_list_find_previous(list->first, valid_index);
    prev->next = link_new(value, prev->next);
    list->size++;
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t value)
{
    /// Insert element at the end of the list
    link_t *link = link_new(value, NULL);
    list->last->next = link;
    /// Update the last pointer to point to the new last element
    list->last = link;
    list->size++;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value)
{
    ioopm_linked_list_insert(list, 0, value);
}

size_t ioopm_linked_list_size(ioopm_list_t *list)
{
    return list->size;
}

elem_t ioopm_linked_list_remove(ioopm_list_t *list, int index)
{
    int valid_index = linked_list_inner_adjust_index(index, ioopm_linked_list_size(list));
    link_t *prev = linked_list_find_previous(list->first, valid_index);
    list->size--;
    return link_remove(prev);
}

elem_t *ioopm_linked_list_get(ioopm_list_t *list, int index)
{
    int valid_index = linked_list_inner_adjust_index(index, ioopm_linked_list_size(list));

    link_t *cursor = linked_list_find_previous(list->first, valid_index);

    return &cursor->next->element;
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
    return (list->size == 0);
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    size_t size = ioopm_linked_list_size(list);
    bool result = true;

    for (int i = 0; i < (int) size && result; i++)
    {
        elem_t current = *ioopm_linked_list_get(list, i);
        result = result && prop(current, current, extra);
    }
    return result;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    size_t size = ioopm_linked_list_size(list);
    bool result = false;

    for (int i = 0; i < (int) size && !result; i++)
    {
        elem_t current = *ioopm_linked_list_get(list, i);
        result = prop(current, current, extra);
    }
    return result;
}

bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element)
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

void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_function fun, void *extra)
{
    size_t size = ioopm_linked_list_size(list);
    link_t *current = list->first->next;

    for (int i = 0; i < (int) size; i++)
    {
        fun(current->element, &current->element, extra);
        current = current->next;
    }
}
