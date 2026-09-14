#include <stdlib.h>

#include "iterator.h"
#include "private.h"
#include "linked_list.h"
#include "../../../src/refmem.h"

// To address a current bug with the refmem implementation as it
// doesnt check if the passed destructor function is a null pointer
static void dummy_destructor(__attribute_maybe_unused__ obj *o) {}

ioopm_list_iterator_t *ioopm_list_iterator_demo(ioopm_list_t *list)
{
    ioopm_list_iterator_t *result = allocate(sizeof(struct iter), dummy_destructor);
    retain(result);

    result->current = list->first;
    result->list = list; /// Iterator remembers where it came from

    return result;
}

bool ioopm_iterator_has_next_demo(ioopm_list_iterator_t *iter)
{
    return iter->current->next != NULL;
}

elem_t ioopm_iterator_next_demo(ioopm_list_iterator_t *iter)
{

    iter->current = iter->current->next;
    return iter->current->element;
}

elem_t ioopm_iterator_current_demo(ioopm_list_iterator_t *iter)
{
    return iter->current->next->element;
}

void ioopm_iterator_destroy_demo(ioopm_list_iterator_t *iter)
{
    release(iter);
}

void ioopm_iterator_reset_demo(ioopm_list_iterator_t *iter)
{
    iter->current = iter->list->first;
}
