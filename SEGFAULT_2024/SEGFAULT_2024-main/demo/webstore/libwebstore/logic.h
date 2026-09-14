#pragma once

#include "hash_table.h"
#include "linked_list.h"
#include "private.h"

typedef struct merch merch_t;
struct merch
{
    char *name;
    char *desc;
    int price;
    int amount_in_carts;
    ioopm_list_t *locs;
};

typedef struct merch_lookup merch_lookup_t;
struct merch_lookup
{
    merch_t *value;
    bool success;
};

typedef struct shelf shelf_t;
struct shelf
{
    char *shelf;
    int quantity;
};

// Typedef for our ware house
typedef struct warehouse ioopm_wh_t;
struct warehouse
{
    ioopm_hash_table_t *merches;
    ioopm_hash_table_t *carts;
    ioopm_hash_table_t *storage_locations;
    int carts_count;
};

typedef struct cart cart_t;
struct cart
{
    int cart_id;
    ioopm_hash_table_t *merches;
};

/// @brief Create a new ware house and allocate memory for it
/// @return A pointer to a new empty ware house
ioopm_wh_t *ioopm_wh_create(void);

/// @brief Delete a warehouse and free its memory
/// @param wh a pointer to a ware house to be destroyed
void ioopm_wh_destroy(ioopm_wh_t *wh);

/// @brief Allocates memory for a merch on the heap and sets the merch properties
/// @param name name of the merch
/// @param desc description of the merch
/// @param price price of the merch
/// @return a pointer to the merch
merch_t *ioopm_make_merch(char *name, char *desc, int price);

/// @brief Takes properties of a merch and adds it in a ware house
/// @param wh the warehouse to add a merch to
/// @param name the name of the merch to be added
/// @param desc a description of the merch to be added
/// @param price the price of the merch to be added
void ioopm_add_merch(ioopm_wh_t *wh, char *name, char *desc, int price);

/// @brief List all merchs in a ware house
/// @param wh the ware house to operate on
void ioopm_print_wh_list(ioopm_wh_t *wh);

/// @brief Takes a name of a merch, delete and free it's memory in a ware house
/// @param name the name of the merch to be deleted
/// @param wh the ware house to operate on
void ioopm_delete_merch(ioopm_wh_t *wh, char *name);

/// @brief Edits an existing merch in a ware house and handle it's allocated memory
/// @param wh the ware house to operate on
/// @param to_edit the name of an existing merch to edit
/// @param name the new name of the merch
/// @param desc a new description of the merch
/// @param price the new price of the merch
/// @param rewrite true if the merch should be rewritten
void ioopm_edit_merch(ioopm_wh_t *wh, char *to_edit, char *name, char *desc, int price, bool rewrite);

/// @brief Increase the stock of a merch with atleast 1
/// @param shelf a new or existing shelf
/// @param name the name of the merch to replensih
/// @param quantity quantity to be replenished
void ioopm_replenish_merch(ioopm_wh_t *wh, char *shelf, char *name, int quantity);

/// @param wh warehouse to be operated on
/// @param name name of merch to look for
/// @return a tuple of a truth-value and if found: the value
merch_lookup_t ioopm_merch_lookup(ioopm_hash_table_t *merches, char *name);

/// @param merches hashtable to be operated on
/// @param name
bool ioopm_has_merch(ioopm_hash_table_t *merches, char *name);

int ioopm_get_stock(ioopm_wh_t *wh, char *merch_name);

/// @brief
/// @param wh
/// @param merch
void ioopm_show_stock(ioopm_wh_t *wh, char *merch);

/// @brief Create an empty shopping cart
/// @param wh the warehouse to add the cart to
int ioopm_create_cart(ioopm_wh_t *wh);

/// @brief Removes a cart with uniqe iid from a warehouse
/// @param wh the warehouse to remove the cart from
/// @param cart_id the id of the cart to be removed
void ioopm_remove_cart(ioopm_wh_t *wh, int cart_id);

/// @brief Add some quantity of a merch to a uniqe cart
/// @param wh the warehouse where the cart exists
/// @param cart_id uniqe id for the cart to operate on
/// @param name the name of the merch to be added to the cart
/// @param quantity quantity of the merch to be added to the cart
void ioopm_add_to_cart(ioopm_wh_t *wh, int cart_id, char *name, int quantity);

/// @brief Rempove some quantity of a merch from a uniqe cart
/// @param wh the warehouse where the cart exists
/// @param cart_id uniqe id for the cart to operate on
/// @param name the name of the merch to added to the cart
/// @param quantity quantity of the merch to be added to the cart
void ioopm_remove_from_cart(ioopm_wh_t *wh, int cart_id, char *name, int quantity);

/// @brief Calculate cost of all merches in cart
/// @param wh the warehouse where the cart exists
/// @param cart_id uniqe id for the cart to operate on
/// @return the sum of all merches in cart
int ioopm_calculate_cost(ioopm_wh_t *wh, int cart_id);

/// @brief Checksout a cart decreasing stock of merches in cart and destry cart
/// @param wh the warehouse to check out from
/// @param cart_id the uniqe id of the cart to be checked out
void ioopm_checkout(ioopm_wh_t *wh, int cart_id);

/// @brief Gets a list with all shelfs in the werehouse which has the merch
/// @param wh the warehouse were the merch and shelfs exist
/// @name name od the merch that we want to find storage locations of
ioopm_list_t *get_locations_from_merch(ioopm_wh_t *wh, char *name);
