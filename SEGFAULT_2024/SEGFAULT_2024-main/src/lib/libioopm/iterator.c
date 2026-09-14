#include <stdlib.h>
#include "iterator.h"
#include "private.h"
#include "linked_list.h"

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
    ioopm_list_iterator_t *result = malloc(sizeof(struct iter));

    result->current = list->first;
    result->list = list; /// Iterator remembers where it came from

    return result;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    return iter->current->next != NULL;
}

elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter)
{

    iter->current = iter->current->next;
    return iter->current->element;
}

elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
    return iter->current->next->element;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
    free(iter);
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
    iter->current = iter->list->first;
}
