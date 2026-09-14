#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "logic.h"
#include "utils.h"
#include "hash_table.h"
#include "../../../src/refmem.h"

static void wh_destructor(obj *o);
static void shelf_destructor(obj *o);
static void cart_destructor(obj *o);
static void mech_destructor(obj *o);

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

static bool string_key_eq(elem_t e1, elem_t e2)
{
    return (strcmp(e1.pointer_value, e2.pointer_value) == 0);
}

static int int_hash(elem_t number)
{
    return number.int_value;
}

static bool int_compare_eq(elem_t e1, elem_t e2)
{
    return (e1.int_value == e2.int_value);
}

ioopm_wh_t *ioopm_wh_create(void)
{
    ioopm_wh_t *wh = allocate(sizeof(ioopm_wh_t), wh_destructor);
    if (!wh) {
        return NULL;
    }
    retain(wh);

    wh->merches = ioopm_hash_table_create_demo(string_sum_hash, string_key_eq, NULL);
    wh->carts = ioopm_hash_table_create_demo(int_hash, int_compare_eq, NULL);
    wh->storage_locations = ioopm_hash_table_create_demo(string_sum_hash, string_key_eq, NULL);
    wh->carts_count = 0;
    return wh;
}

merch_t *ioopm_make_merch(char *name, char *desc, int price)
{
    merch_t *result = allocate(sizeof(merch_t), mech_destructor);
    if (!result) {
        return NULL;
    }
    retain(result);

    result->name = name;
    result->desc = desc;
    result->price = price;
    result->amount_in_carts = 0;
    result->locs = ioopm_linked_list_create_demo(string_key_eq); // merch not in stock yet
    return result;
}

static void mech_destructor(obj *o) {
    merch_t *merch = (merch_t *) o;
    release(merch->name);
    release(merch->desc);
    ioopm_linked_list_destroy_demo(merch->locs);
}

static void free_merch(merch_t *elem)
{
    release(elem);
}

static void shelf_destructor(obj *o) {
    shelf_t *shelf = (shelf_t *) o;
    release(shelf->shelf);
}

static void free_shelf(__attribute_maybe_unused__ elem_t key, elem_t *value, __attribute_maybe_unused__ void *arg)
{
    shelf_t *shelf_ptr = value->pointer_value;
    release(shelf_ptr);
}

static void ioopm_delete_merch_ht(ioopm_hash_table_t *merches, char *name)
{
    merch_lookup_t lookup_merch = ioopm_merch_lookup(merches, name);
    merch_t *merch = lookup_merch.value;
    if (lookup_merch.success)
    {
        if (merch->amount_in_carts == 0)
        {
            {
                merch_t *tmp = ioopm_hash_table_remove_demo(merches, ptr_elem(name)).value.pointer_value;
                if (tmp->locs)
                {
                    ioopm_linked_list_apply_to_all_demo(tmp->locs, free_shelf, NULL);
                }
                free_merch(tmp);
            }
        }
        else
        {
            release(name);
            printf("Merch is in cart - can not be edited\n");
        }
    }
}

static void ioopm_delete_merch_final(ioopm_hash_table_t *merches, char *name)
{
    merch_lookup_t lookup_merch = ioopm_merch_lookup(merches, name);
    if (lookup_merch.success)
    {
        merch_t *tmp = ioopm_hash_table_remove_demo(merches, ptr_elem(name)).value.pointer_value;
        if (tmp->locs)
        {
            ioopm_linked_list_apply_to_all_demo(tmp->locs, free_shelf, NULL);
        }
        free_merch(tmp);
    }
}

void ioopm_delete_merch(ioopm_wh_t *wh, char *name)
{
    ioopm_delete_merch_ht(wh->merches, name);
}

static void destroy_merches(ioopm_hash_table_t *merches)
{
    ioopm_list_t *keys = ioopm_hash_table_keys_demo(merches);

    // destroying hash_table of merches (loop through and destroy every element in linked_list)
    link_t *curr = keys->first->next;
    link_t *next;
    while (curr)
    {
        next = curr->next; // saves pointer to next element before destroying current element
        ioopm_delete_merch_final(merches, (char *)curr->element.pointer_value);
        curr = next;
    }
    ioopm_linked_list_destroy_demo(keys);
    ioopm_hash_table_destroy_demo(merches);
}

static void cart_destructor(obj *o) {
    cart_t *cart = (cart_t *) o;
    ioopm_hash_table_destroy_demo(cart->merches);
}

static void destroy_cart(cart_t *cart)
{
    release(cart);
}

static void destroy_carts(ioopm_wh_t *wh)
{
    // destroying hash_table with carts (loop through and destroy every hashtable)
    ioopm_list_t *carts = ioopm_hash_table_values_demo(wh->carts);
    link_t *curr = carts->first->next;
    link_t *next;
    while (curr)
    {
        next = curr->next; // saves pointer to next element before destroying current element
        destroy_cart((cart_t *)(curr->element.pointer_value));
        curr = next;
    }
    ioopm_linked_list_destroy_demo(carts);
    ioopm_hash_table_destroy_demo(wh->carts);
}

static void destroy_storage_locations(ioopm_wh_t *wh)
{
    ioopm_hash_table_destroy_demo(wh->storage_locations);
}

static void wh_destructor(obj *o) {
    ioopm_wh_t *wh = (ioopm_wh_t *) o;
    destroy_carts(wh);
    destroy_merches(wh->merches);
    destroy_storage_locations(wh);
}

void ioopm_wh_destroy(ioopm_wh_t *wh)
{
    release(wh);
}

bool ioopm_has_merch(ioopm_hash_table_t *merches, char *name)
{
    return ioopm_hash_table_has_key_demo(merches, ptr_elem(name));
}

merch_lookup_t ioopm_merch_lookup(ioopm_hash_table_t *merches, char *name)
{
    option_t lookup_hash = ioopm_hash_table_lookup_demo(merches, ptr_elem(name));
    merch_lookup_t lookup = {.success = lookup_hash.success, .value = lookup_hash.value.pointer_value};
    return lookup;
}

void ioopm_add_merch(ioopm_wh_t *wh, char *name, char *desc, int price)
{

    if (!ioopm_has_merch(wh->merches, name))
    {
        merch_t *new_merch = ioopm_make_merch(name, desc, price);
        ioopm_hash_table_insert_demo(wh->merches, ptr_elem(name), ptr_elem(new_merch));
    }
    else
    {
        release(name);
        release(desc);
        printf("Merch already exists! \n");
    }
}

void ioopm_print_wh_list(ioopm_wh_t *wh)
{
    ioopm_list_t *lst = ioopm_hash_table_keys_demo(wh->merches);
    assert(lst);
    link_t *curr_elem = lst->first->next;
    printf("Merch: \n");
    bool run = true;
    while (run)
    {
        for (int count = 0; count < 20; count++)
        {
            if (curr_elem == NULL)
            {
                ioopm_linked_list_destroy_demo(lst);
                return;
            }
            else
            {
                printf("\t%s \n", (char *)curr_elem->element.pointer_value);
                curr_elem = curr_elem->next;
            }
        }
        run = !ask_confirm("Do you want to print more", 'n'); // returns true when user wants to stop printing
    }
    printf("\n");
    ioopm_linked_list_destroy_demo(lst);
}

void ioopm_edit_merch(ioopm_wh_t *wh, char *to_edit, char *name, char *desc, int price, bool rewrite)
{
    merch_lookup_t lookup_merch = ioopm_merch_lookup(wh->merches, to_edit);
    merch_t *merch = lookup_merch.value;
    if (lookup_merch.success)
    {
        if (merch->amount_in_carts == 0)
        {
            if (rewrite)
            {
                ioopm_add_merch(wh, name, desc, price);
                ioopm_delete_merch(wh, to_edit);
                release(to_edit);
            }
            else
            {
                release(to_edit);
                release(name);
                release(desc);
            }
        }
        else
        {
            release(to_edit);
            release(name);
            release(desc);
            printf("merch is in cart - can not be edited\n");
        }
    }
    else
    {
        release(to_edit);
        release(name);
        release(desc);
        printf("Error.. Merch not found, plese try again! \n");
    }
}

/*_______stock and shelf section_______*/

__attribute_maybe_unused__
static void print_locs(ioopm_list_t *lst)
{
    assert(lst);
    link_t *curr = lst->first->next;
    if (curr)
    {
        while (curr != NULL)
        {

            shelf_t *locs = curr->element.pointer_value;
            printf("%s\t:%d\n", locs->shelf, locs->quantity);
            curr = curr->next;
        }
    }
    else
    {
        printf("Merch not in stock!\n");
    }
}

// The storage locations are a linked list
ioopm_list_t *get_locations_from_merch(ioopm_wh_t *wh, char *name)
{
    merch_lookup_t lookup = ioopm_merch_lookup(wh->merches, name);
    if (lookup.success)
    {
        return lookup.value->locs;
    }
    else
    {
        printf("Error.. Merch not found, plese try again!\n");
        return NULL;
    }
}

static void sum_stock(elem_t value, __attribute_maybe_unused__ elem_t *ignore, void *sum)
{
    shelf_t *shelf = value.pointer_value;
    *(int *)sum = *(int *)sum + shelf->quantity;
}

int ioopm_get_stock(ioopm_wh_t *wh, char *merch_name)
{
    int stock = 0;
    ioopm_list_t *locs = get_locations_from_merch(wh, merch_name);
    ioopm_linked_list_apply_to_all_demo(locs, sum_stock, &stock);
    return stock;
}

void ioopm_show_stock(ioopm_wh_t *wh, char *merch)
{
    ioopm_list_t *stock = get_locations_from_merch(wh, merch);
    if (!ioopm_has_merch(wh->merches, merch))
    {
        printf("Merch does not exist.\n");
        release(merch);
        return;
    }

    if (ioopm_linked_list_size_demo(stock) == 0)
    {
        printf("Merch not in stock.\n");
        release(merch);
        return;
    }
    else
    {
        link_t *cursor = stock->first->next;
        shelf_t *shelf;
        for (int i = 0; i < (int) stock->size; i++)
        {
            shelf = cursor->element.pointer_value;
            printf("Shelf: %s: \t %d \n", shelf->shelf, shelf->quantity);
            cursor = cursor->next;
        }
    }

    release(merch);
}

static option_t shelf_lookup(ioopm_wh_t *wh, char *shelf_id)
{
    option_t lookup = ioopm_hash_table_lookup_demo(wh->storage_locations, ptr_elem(shelf_id));

    if (lookup.success)
    {
        merch_t *merch = lookup.value.pointer_value;
        ioopm_list_t *locations = merch->locs;
        ioopm_eq_function comp_func = locations->eq_func;
        for (int i = 0; i < (int) locations->size; i++)
        {
            shelf_t *shelf = ioopm_linked_list_get_demo(locations, i)->pointer_value;
            if (comp_func(ptr_elem(shelf->shelf), ptr_elem(shelf_id)))
            {
                lookup.value.pointer_value = shelf;
                return lookup;
            }
        }
    }
    return lookup;
}

static shelf_t *make_shelf(char *shelf, int quantity)
{
    shelf_t *new_shelf = allocate(sizeof(shelf_t), shelf_destructor);
    if (!new_shelf) {
        return NULL;
    }
    retain(new_shelf);

    new_shelf->shelf = shelf;
    new_shelf->quantity = quantity;
    return new_shelf;
}

void ioopm_replenish_merch(ioopm_wh_t *wh, char *shelf_id, char *name, int quantity)
{
    merch_lookup_t merch_lookup = ioopm_merch_lookup(wh->merches, name);
    release(name);
    if (quantity <= 0) // to low quantity
    {
        release(shelf_id);
        return;
    }
    if (!merch_lookup.success) // merch does not exist
    {
        printf("Error.. Merch not found, plese try again!\n");
        release(shelf_id);
        return;
    }
    option_t lookup = shelf_lookup(wh, shelf_id);
    if (lookup.success) // merch exists -> replenish
    {
        release(shelf_id);
        shelf_t *shelf = lookup.value.pointer_value;
        shelf->quantity = shelf->quantity + quantity;
    }
    else // shelf does not exist -> create shelf
    {
        shelf_t *new_shelf = make_shelf(shelf_id, quantity);
        ioopm_hash_table_insert_demo(wh->storage_locations, ptr_elem(shelf_id), ptr_elem(merch_lookup.value));  // adds new shelf to storage locations
        ioopm_linked_list_append_demo(merch_lookup.value->locs, ptr_elem(new_shelf));                              // adds new shelf to list of locations of merch
    }
}

static void decrease_stock(ioopm_wh_t *wh, char *name, int quantity)
{
    ioopm_list_t *locs = get_locations_from_merch(wh, name);
    link_t *current_shelf = locs->first->next;
    shelf_t *shelf_ptr = current_shelf->element.pointer_value;
    merch_t *merch = ioopm_merch_lookup(wh->merches, name).value;
    for (int i = 0; i < quantity; i++)
    {
        merch->amount_in_carts--;
        shelf_ptr->quantity--;
        if (shelf_ptr->quantity == 0)
        {
            ioopm_hash_table_remove_demo(wh->storage_locations, ptr_elem(shelf_ptr->shelf));

            shelf_t *empty_shelf = ioopm_linked_list_remove_demo(merch->locs, 0).pointer_value;
            release(empty_shelf);
            if (locs->first->next)
            {
                current_shelf = locs->first->next;
                shelf_ptr = current_shelf->element.pointer_value;
            }
        }
    }
}

/*_______cart section_______*/

int ioopm_create_cart(ioopm_wh_t *wh)
{
    cart_t *shopping_cart = allocate(sizeof(cart_t), cart_destructor); // skapar ny cart
    if (!shopping_cart) {
        return -1;
    }
    retain(shopping_cart);

    int cart_id = wh->carts_count;

    shopping_cart->merches = ioopm_hash_table_create_demo(string_sum_hash, string_key_eq, int_compare_eq); // skapar ny hashtable i cart för merches
    shopping_cart->cart_id = cart_id;
    ioopm_hash_table_insert_demo(wh->carts, int_elem(cart_id), ptr_elem(shopping_cart));
    wh->carts_count++; // uppdatera även i wh, indexeras från 0
    return cart_id;
}

void ioopm_remove_cart(ioopm_wh_t *wh, int cart_id)
{
    option_t lookup = ioopm_hash_table_remove_demo(wh->carts, int_elem(cart_id));
    if (lookup.success)
    {
        cart_t *cart = lookup.value.pointer_value;
        destroy_cart(cart);
    }
    else
    {
        print("Cart does not exist\n");
    }
}

static int avaliable_stock(ioopm_wh_t *wh, merch_t *merch)
{
    return ioopm_get_stock(wh, merch->name) - merch->amount_in_carts;
}

void ioopm_add_to_cart(ioopm_wh_t *wh, int cart_id, char *name, int quantity)
{
    if (quantity > 0)
    {
        option_t lookup_cart = ioopm_hash_table_lookup_demo(wh->carts, int_elem(cart_id));
        if (lookup_cart.success)
        {
            merch_lookup_t lookup_merch = ioopm_merch_lookup(wh->merches, name);
            if (lookup_merch.success)
            {
                cart_t *cart = lookup_cart.value.pointer_value;
                merch_t *merch = lookup_merch.value;
                int available = avaliable_stock(wh, merch);
                if (quantity <= available)
                {
                    // printf("ADDED %s\t%d\n", merch->name, quantity);
                    merch->amount_in_carts += quantity;
                    ioopm_hash_table_insert_demo(cart->merches, ptr_elem(merch->name), int_elem(quantity));
                }
                else
                {
                    printf("Not enough stock available!\n");
                }
            }
            else
            {
                printf("Error.. merch does not exist!\n");
            }
        }
        else
        {
            printf("Error.. cart does not exist!\n");
        }
    }
    release(name);
}

void ioopm_remove_from_cart(ioopm_wh_t *wh, int cart_id, char *name, int quantity)
{
    if (quantity > 0)
    {
        option_t lookup_cart = ioopm_hash_table_lookup_demo(wh->carts, int_elem(cart_id));
        if (lookup_cart.success)
        {
            cart_t *cart = lookup_cart.value.pointer_value;
            option_t lookup_merch_in_cart = ioopm_hash_table_lookup_demo(cart->merches, ptr_elem(name));
            if (lookup_merch_in_cart.success)
            {
                merch_t *merch = ioopm_merch_lookup(wh->merches, name).value;
                int quantity_in_cart = lookup_merch_in_cart.value.int_value;
                if (quantity == quantity_in_cart)
                {
                    // printf("REMOVED %s\t%d\n", merch->name, quantity);
                    merch->amount_in_carts -= quantity;
                    ioopm_hash_table_remove_demo(cart->merches, ptr_elem(name));
                }
                else if (quantity < quantity_in_cart)
                {
                    merch->amount_in_carts -= quantity;
                    int new_quantity = quantity_in_cart - quantity;
                    ioopm_hash_table_insert_demo(cart->merches, ptr_elem(name), int_elem(new_quantity));
                }
                else
                {

                    printf("Trying to remove too big amount!\n");
                }
            }
            else
            {

                printf("Error.. merch does not exist in cart!\n");
            }
        }
        else
        {

            printf("Error.. cart does not exist!\n");
        }
    }
    // if quantity == 0 -> just free name
    release(name);
}

static int get_merch_price(ioopm_wh_t *wh, char *name)
{
    return ioopm_merch_lookup(wh->merches, name).value->price;
}

static int get_merch_quantity_in_cart(ioopm_hash_table_t *cart, char *name)
{
    return ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).value.int_value;
}

static int sum_merches_in_cart(ioopm_wh_t *wh, ioopm_hash_table_t *cart)
{
    ioopm_list_t *list_merches_in_cart = ioopm_hash_table_keys_demo(cart);
    link_t *curr = list_merches_in_cart->first->next;
    char *curr_merch;
    int quantity;
    int price;
    int cart_total = 0;
    while (curr)
    {
        curr_merch = curr->element.pointer_value;
        quantity = get_merch_quantity_in_cart(cart, curr_merch);
        price = get_merch_price(wh, curr_merch);
        cart_total += quantity * price;
        curr = curr->next;
    }

    ioopm_linked_list_destroy_demo(list_merches_in_cart);

    return cart_total;
}

int ioopm_calculate_cost(ioopm_wh_t *wh, int cart_id)
{
    option_t cart_lookup = ioopm_hash_table_lookup_demo(wh->carts, int_elem(cart_id));
    if (cart_lookup.success)
    {
        cart_t *cart = cart_lookup.value.pointer_value;
        int sum = sum_merches_in_cart(wh, cart->merches);
        return sum;
    }
    else
    {
        print("Cart does not exist\n");
        return -1;
    }
    return 0;
}

static void checkout_remove_merches(ioopm_wh_t *wh, cart_t *cart)
{

    ioopm_list_t *list_merches_in_cart = ioopm_hash_table_keys_demo(cart->merches);
    link_t *curr = list_merches_in_cart->first->next;
    char *curr_merch;
    int quantity;
    while (curr)
    {
        curr_merch = curr->element.pointer_value;
        quantity = get_merch_quantity_in_cart(cart->merches, curr_merch);

        decrease_stock(wh, curr_merch, quantity);

        curr = curr->next;
    }

    ioopm_linked_list_destroy_demo(list_merches_in_cart);
}

void ioopm_checkout(ioopm_wh_t *wh, int cart_id)
{
    option_t cart_lookup = ioopm_hash_table_lookup_demo(wh->carts, int_elem(cart_id));
    if (cart_lookup.success)
    {
        cart_t *cart = cart_lookup.value.pointer_value;
        if (ioopm_hash_table_size_demo(cart->merches) == 0)
        {
            printf("Cart is empty \n");
        }
        else
        {
            checkout_remove_merches(wh, cart);
        }
        ioopm_hash_table_remove_demo(wh->carts, int_elem(cart->cart_id));
        destroy_cart(cart);
    }
    else
    {
        printf("Cart does not exist\n");
    }
}
