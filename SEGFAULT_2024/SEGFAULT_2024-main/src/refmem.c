#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "refmem.h"

#include "lib/libioopm/linked_list.h"
#include "lib/libmeta/meta.h"

static ioopm_list_t *list; // The linked list where objects are stored with their respective reference count.

// Creates the linked list to store objects into.
// This function must be called at the start of the program to work properly.
// Returns if completed correctly.
static void __attribute__((constructor)) startup(void)
{
    list = ioopm_linked_list_create(NULL);
    return;
}

// Decreases the reference count of an object
// If the object is null return without changing reference count.
// Always returns void
void retain(obj *o)
{
    if (o == NULL)
    {
        return;
    }
    else
    {
        obj_to_meta(o)->ref_count++;
        return;
    }
}

// Decreases the reference count of an object by 1.
// If object is NULL or has reference count 0 then release does nothing.
// If objects has reference count 1 then that object is considered garbage and subsequently freed.
// Always returns void.
void release(obj *o)
{
    if (o == NULL)
    {
        return;
    }
    else if (obj_to_meta(o)->ref_count == 0)
    {
        return;
    }
    else if (obj_to_meta(o)->ref_count == 1)
    {
        obj_to_meta(o)->ref_count--;
        deallocate(o);
        return;
    }
    else
    {
        obj_to_meta(o)->ref_count--;
        return;
    }
}

// Returns the current reference count of an object o.
// If the object is NULL, -1 is returned.
size_t rc(obj *o)
{
    if (o == NULL)
    {
        return -1;
    }

    return obj_to_meta(o)->ref_count;
}

obj *allocate(size_t bytes, function1_t destructor)
{
    // Uses malloc to allocate sapce for the meta data + the bytes needed by the user
    meta_data *object = malloc(sizeof(meta_data) + bytes);
    if (!object)
    {
        return object;
    }
    object->ref_count = 0;
    object->type = NULL;
    object->destroy = destructor;

    return meta_to_obj(object);
}

obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor)
{
    // Uses calloc to allocate space for the entire block_size
    size_t block_size = sizeof(meta_data) + elements * elem_size;
    meta_data *object = calloc(1, block_size);
    if (!object)
    {
        return object;
    }
    object->ref_count = 0;
    object->type = NULL;
    object->destroy = destructor;
    return meta_to_obj(object);
}

// Checks an objects rc value and frees it if the value is zero.
// Any amount other thant zero results in a fail and segfault.
// Runs the objects destructor if it has any
// Always returns void if fully completed
void deallocate(obj *o)
{
    assert(rc(o) == 0);
    meta_data *header = obj_to_meta(o);
    if (header->destroy)
    {
        header->destroy(o);
    }
    free(obj_to_meta(o));
    return;
}

void set_cascade_limit(size_t limit)
{
    (void)limit;
}

size_t get_cascade_limit(void)
{
    return 0;
}

// Ioopm predicate function to check if any of the objects in the list have reference count zero.
// If any object has reference count zero the functions starts to loop through all elements
// and calls deallocate on the "garbage" objects.
// If the list is empty or no object has reference count zero the function returns without any action.
// Always returns void if fully completed
void cleanup(void)
{
    size_t size = ioopm_linked_list_size(list);

    if (ioopm_linked_list_is_empty(list))
    {
        return;
    }
    else
    {
        for (int i = size - 1; i > 0; i--)
        {
            if (rc((obj *)ioopm_linked_list_get(list, i)->pointer_value) == 0)
            {
                obj *removed = ioopm_linked_list_remove(list, i).pointer_value;
                deallocate(removed);
            }
        }
        return;
    }
}

// Loops through all objects in the list
// Recursively calls release on the objects untill they have reference count 1
// Calls release one final time on objects with reference count 1 (which in turn calls deallocate and deletes them)
// Deletes the list once all objects in the list have been deleted
void __attribute__((destructor)) shutdown(void)
{
    size_t size = ioopm_linked_list_size(list);
    for (int i = size - 1; i > 0; i--)  {
        obj *garbage = ioopm_linked_list_remove(list, i).pointer_value;
        while(rc(garbage) > 1)  {
            release(garbage);
        }
        release(garbage);
    }
    ioopm_linked_list_destroy(list);
    return;
}
