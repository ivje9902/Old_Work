#include <CUnit/Basic.h>

#include "../libwebstore/logic.h"
#include "../libwebstore/utils.h"

static int init_suite(void)
{
    // Change this function if you want to do something *before* you
    // run a test suite
    return 0;
}

static int clean_suite(void)
{
    // Change this function if you want to do something *after* you
    // run a test suite
    return 0;
}

// These are example test functions. You should replace them with
// functions of your own.
static void test_create_destroy_wh(void)
{
    ioopm_wh_t *wh = ioopm_wh_create();
    CU_ASSERT_PTR_NOT_NULL(wh);
    ioopm_wh_destroy(wh);
}

static void test_add_merch(void)
{
    ioopm_wh_t *wh = ioopm_wh_create();
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    CU_ASSERT_TRUE(ioopm_hash_table_lookup_demo(wh->merches, ptr_elem(name)).success);

    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(wh->merches), 1);

    ioopm_wh_destroy(wh);
}

static void test_has_merch(void)
{
    ioopm_wh_t *wh = ioopm_wh_create();
    CU_ASSERT_FALSE(ioopm_has_merch(wh->merches, "skor"));

    // creates new merch and inserts it
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    bool truth_value = ioopm_has_merch(wh->merches, name);
    CU_ASSERT_TRUE(truth_value);
    ioopm_wh_destroy(wh);
}

static void test_edit_merch()
{
    ioopm_wh_t *wh = ioopm_wh_create();

    // inserting element
    char *existing_merch = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(existing_merch), duplicate_string(desc), price);

    // try to edit a merch which doesent exist
    char *non_existing_merch = "hej";
    char *new_merch = "tröja";
    ioopm_edit_merch(wh, duplicate_string(non_existing_merch), duplicate_string(new_merch), duplicate_string("stor tröja"), 200, true);
    bool has_non_existing_merch = ioopm_has_merch(wh->merches, non_existing_merch);
    bool has_new_merch = ioopm_has_merch(wh->merches, new_merch);
    CU_ASSERT_FALSE(has_non_existing_merch);
    CU_ASSERT_FALSE(has_new_merch);

    // try to edit a merch that exist
    ioopm_edit_merch(wh, duplicate_string(existing_merch), duplicate_string(new_merch), duplicate_string("stor tröja"), 200, true);
    bool has_existing_merch = ioopm_has_merch(wh->merches, existing_merch);
    has_new_merch = ioopm_has_merch(wh->merches, new_merch);
    CU_ASSERT_FALSE(has_existing_merch);
    CU_ASSERT_TRUE(has_new_merch);

    ioopm_wh_destroy(wh);
}

static void test_edit_merch_rewrite()
{
    ioopm_wh_t *wh = ioopm_wh_create();

    // inserting merch
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);

    // editing valid merch with rewrite set to false
    char *new_name = "tröja";
    char *new_desc = "stor tröja";
    ioopm_edit_merch(wh, duplicate_string(name), duplicate_string(new_name), duplicate_string(new_desc), price, false);
    bool overwrite = ioopm_has_merch(wh->merches, new_name);
    CU_ASSERT_FALSE(overwrite);
    ioopm_wh_destroy(wh);
}

static bool find_shelf(__attribute_maybe_unused__ elem_t ignored, elem_t shelf, void *ref)
{
    char *shelf_ref = (char *)ref;
    shelf_t *shelf_found = shelf.pointer_value;
    return (!strcmp(shelf_found->shelf, shelf_ref));
}

static bool find_shelf_quantity(__attribute_maybe_unused__ elem_t ignored, elem_t shelf, void *ref)
{
    int qty_ref = *(int *)ref;
    shelf_t *shelf_found = shelf.pointer_value;
    return shelf_found->quantity == qty_ref;
}

static void test_replenish()
{
    ioopm_wh_t *wh = ioopm_wh_create();

    // inserting merch
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    merch_t *merch = ioopm_merch_lookup(wh->merches, name).value;
    char *shelf = "A1";

    // replenish on non existing merch should not succed
    ioopm_replenish_merch(wh, duplicate_string(shelf), duplicate_string("fel"), 10);
    CU_ASSERT_FALSE(ioopm_linked_list_any_demo(merch->locs, find_shelf, shelf));

    // insert 0 shuld not succed
    ioopm_replenish_merch(wh, duplicate_string(shelf), duplicate_string(name), 0);
    CU_ASSERT_FALSE(ioopm_linked_list_any_demo(merch->locs, find_shelf, shelf));

    // insert > 0 should succed
    // quantity should be 12
    int qty = 12;
    ioopm_replenish_merch(wh, duplicate_string(shelf), duplicate_string(name), qty);
    CU_ASSERT_TRUE(ioopm_linked_list_any_demo(merch->locs, find_shelf, shelf));
    CU_ASSERT_TRUE(ioopm_linked_list_any_demo(merch->locs, find_shelf_quantity, &qty));

    // replenish same shelf should increase quantity
    // quantity should be 24
    ioopm_replenish_merch(wh, duplicate_string(shelf), duplicate_string(name), qty);
    qty = qty + qty;
    CU_ASSERT_TRUE(ioopm_linked_list_any_demo(merch->locs, find_shelf_quantity, &qty));

    ioopm_wh_destroy(wh);
}

static void test_create_cart() // Create a cart and check that its empty
{
    ioopm_wh_t *wh = ioopm_wh_create();
    char *merch_name = "A";
    ioopm_add_merch(wh, duplicate_string(merch_name), duplicate_string("test merch"), 50);
    int id = ioopm_create_cart(wh);
    ioopm_hash_table_t *carts = wh->carts;
    cart_t *cart = ioopm_hash_table_lookup_demo(carts, int_elem(id)).value.pointer_value;
    int cartsize = ioopm_hash_table_size_demo(cart->merches);
    CU_ASSERT_EQUAL(0, cartsize);
    CU_ASSERT(wh->carts_count == 1);
    ioopm_wh_destroy(wh);
}

static void test_remove_cart()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    int id = ioopm_create_cart(wh);
    option_t lookup = ioopm_hash_table_lookup_demo(wh->carts, int_elem(id));
    CU_ASSERT_TRUE(lookup.success);
    CU_ASSERT_NOT_EQUAL(wh->carts_count, 0); // make sure cart was created
    ioopm_remove_cart(wh, 1);

    ioopm_remove_cart(wh, id);
    lookup = ioopm_hash_table_lookup_demo(wh->carts, int_elem(id));
    CU_ASSERT_FALSE(lookup.success);
    ioopm_wh_destroy(wh);
}

static void test_get_stock()
{
    ioopm_wh_t *wh = ioopm_wh_create();

    // inserting merch
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 3);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name), 4);
    CU_ASSERT_EQUAL(ioopm_get_stock(wh, name), 7);
    ioopm_wh_destroy(wh);
}

static ioopm_hash_table_t *get_cart(ioopm_wh_t *wh, int cart_id)
{
    cart_t *cart = ioopm_hash_table_lookup_demo(wh->carts, int_elem(cart_id)).value.pointer_value;
    return cart->merches;
}

static void test_add_to_cart()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    // inserting merch
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 3);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name), 4);

    int cart_id = ioopm_create_cart(wh);
    ioopm_hash_table_t *cart = get_cart(wh, cart_id);
    CU_ASSERT_PTR_NOT_NULL(cart);

    // Add to non existing cart
    ioopm_add_to_cart(wh, 1, duplicate_string(name), 1);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup_demo(wh->carts, int_elem(1)).success);

    // Add non existing merch and check that size of cart is 0
    ioopm_add_to_cart(wh, cart_id, duplicate_string("none"), 7);
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(cart), 0);

    // Add 8 of existing merch
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 8);

    // Check that merch is not in cart since avaliable stock is less than asked quantity
    CU_ASSERT_FALSE(ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).success);
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(cart), 0);

    // Add 4 of existing merch
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 4);

    // Check that cart size is 1
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(cart), 1);

    // Check that merch exists in cart
    CU_ASSERT_TRUE(ioopm_hash_table_has_key_demo(cart, ptr_elem(name)));

    // Check that quantity matches 4
    CU_ASSERT_EQUAL(ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).value.int_value, 4);

    // Check that amount_in_carts is 4 for merch
    CU_ASSERT_EQUAL(ioopm_merch_lookup(wh->merches, name).value->amount_in_carts, 4);

    ioopm_wh_destroy(wh);
}

static void test_remove_from_cart()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    // inserting merch
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 1299;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 3);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name), 4);

    int cart_id = ioopm_create_cart(wh);
    ioopm_hash_table_t *cart = get_cart(wh, cart_id);
    merch_t *merch = ioopm_merch_lookup(wh->merches, name).value;

    // Add 4 of existing merch
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 4);
    CU_ASSERT_EQUAL(merch->amount_in_carts, 4);

    // Remove from non existing cart
    int non_existing_cart = 2;
    ioopm_remove_from_cart(wh, non_existing_cart, duplicate_string(name), 4);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup_demo(wh->carts, int_elem(2)).success);
    CU_ASSERT_TRUE(ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).success);                        // merch should still exist in cart 1
    CU_ASSERT_EQUAL(merch->amount_in_carts, 4);                                                        // counter in warehouse merches should stay the same
    CU_ASSERT_EQUAL((long) ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).value.pointer_value, 4); // quantity in cart should stay the same

    // Remove merch that is not in cart
    ioopm_remove_from_cart(wh, cart_id, duplicate_string("merch2"), 4);
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(cart), 1); // merches in cart should stay the same

    // Removing to many
    ioopm_remove_from_cart(wh, cart_id, duplicate_string(name), 5);
    CU_ASSERT_TRUE(ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).success);                        // merch should still exist in cart
    CU_ASSERT_EQUAL(merch->amount_in_carts, 4);                                                        // counter in warehouse merches should stay the same
    CU_ASSERT_EQUAL((long) ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).value.pointer_value, 4); // quantity in cart should stay the same
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(cart), 1);

    // Remove part of quantity
    ioopm_remove_from_cart(wh, cart_id, duplicate_string(name), 2);
    CU_ASSERT_EQUAL(ioopm_hash_table_lookup_demo(cart, ptr_elem(name)).value.int_value, 2); // quantity in cart should be updated
    CU_ASSERT_EQUAL(merch->amount_in_carts, 2);                                             // counter in warehouse merches should be updated
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(cart), 1);                                   // merch should still be in cart

    // Remove remaining quantity of a merch in a cart
    ioopm_remove_from_cart(wh, cart_id, duplicate_string(name), 2);
    CU_ASSERT_FALSE(ioopm_hash_table_has_key_demo(cart, ptr_elem(name))); // merch should be removed from cart
    CU_ASSERT_EQUAL(merch->amount_in_carts, 0);                           // counter in warehouse merches should be updated
    CU_ASSERT_EQUAL(ioopm_hash_table_size_demo(cart), 0);                 // no merches left in cart

    ioopm_wh_destroy(wh);
}

static void test_calculate_cost()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    // inserting merch
    char *name = "skor";
    char *name2 = "vantar";
    char *desc = "fina vita skor";
    int price = 100;
    int price2 = 200;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_add_merch(wh, duplicate_string(name2), duplicate_string(desc), price2);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name2), 10);

    int cart_id = ioopm_create_cart(wh);

    // Getting sum of non-existing cart
    int non_existing_cart = 1;
    CU_ASSERT_EQUAL(ioopm_calculate_cost(wh, non_existing_cart), -1);

    // Add 0 of existing merch to cart sum should be 0
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 0);
    CU_ASSERT_EQUAL(ioopm_calculate_cost(wh, cart_id), 0);

    // Add 2 of skor to cart sum should be 200
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 2);
    CU_ASSERT_EQUAL(ioopm_calculate_cost(wh, cart_id), 200);

    // Add 2 of vantar to cart sum should be 200 + 400 = 600
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name2), 2);
    CU_ASSERT_EQUAL(ioopm_calculate_cost(wh, cart_id), 600);

    // Remove 2 of skor, sum should be 400
    ioopm_remove_from_cart(wh, cart_id, duplicate_string(name), 2);
    CU_ASSERT_EQUAL(ioopm_calculate_cost(wh, cart_id), 400);

    ioopm_wh_destroy(wh);
}

static void test_checkout_empty_cart()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 100;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 10);

    int cart_id = ioopm_create_cart(wh);

    // Check that cart exists
    CU_ASSERT_TRUE(ioopm_hash_table_has_key_demo(wh->carts, int_elem(cart_id)));

    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 12);

    ioopm_checkout(wh, 1);

    // Check that cart still exists
    CU_ASSERT_TRUE(ioopm_hash_table_has_key_demo(wh->carts, int_elem(cart_id)));

    ioopm_checkout(wh, cart_id);

    // Check that cart is destroyed
    CU_ASSERT_FALSE(ioopm_hash_table_has_key_demo(wh->carts, int_elem(cart_id)));

    // Check that stock is still 10
    CU_ASSERT_EQUAL(ioopm_get_stock(wh, name), 10);

    ioopm_wh_destroy(wh);
}

static void test_checkout()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    // inserting merch
    char *name = "skor";
    char *name2 = "vantar";
    char *desc = "fina vita skor";
    int price = 100;
    int price2 = 200;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_add_merch(wh, duplicate_string(name2), duplicate_string(desc), price2);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A3"), duplicate_string(name2), 10);

    int cart_id = ioopm_create_cart(wh);

    // Test to decrease stock in on different shelfs
    // Takes 10 from first shelf, 2 from second
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 12);

    ioopm_checkout(wh, cart_id);

    // Check that cart is destroyed
    CU_ASSERT_FALSE(ioopm_hash_table_has_key_demo(wh->carts, int_elem(cart_id)));

    // Check that 12 of skor is deleted from wh and that locations id only 1
    merch_lookup_t merch_lookup = ioopm_merch_lookup(wh->merches, name);
    CU_ASSERT_TRUE(merch_lookup.success); // merch should still exist
    merch_t *merch = merch_lookup.value;
    CU_ASSERT_EQUAL(ioopm_linked_list_size_demo(merch->locs), 1);
    CU_ASSERT_EQUAL(ioopm_get_stock(wh, name), 8);
    CU_ASSERT_EQUAL(merch->amount_in_carts, 0);

    ioopm_wh_destroy(wh);
}

static void test_checkout_2_merches()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    // inserting merch
    char *name = "skor";
    char *name2 = "vantar";
    char *desc = "fina vita skor";
    int price = 100;
    int price2 = 200;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_add_merch(wh, duplicate_string(name2), duplicate_string(desc), price2);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A3"), duplicate_string(name2), 10);

    int cart_id = ioopm_create_cart(wh);

    // Test to decrease stock in on different shelfs
    // Takes 10 from first shelf, 2 from second
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name), 20);
    ioopm_add_to_cart(wh, cart_id, duplicate_string(name2), 8);

    ioopm_checkout(wh, cart_id);

    // Check that cart is destroyed
    CU_ASSERT_FALSE(ioopm_hash_table_has_key_demo(wh->carts, int_elem(cart_id)));

    // Check that all storage locations for skor is deleted from wh
    merch_lookup_t merch_lookup = ioopm_merch_lookup(wh->merches, name);
    CU_ASSERT_TRUE(merch_lookup.success); // merch should still exist
    merch_t *merch = merch_lookup.value;
    CU_ASSERT_EQUAL(ioopm_linked_list_size_demo(merch->locs), 0);
    CU_ASSERT_EQUAL(ioopm_get_stock(wh, name), 0);

    // Check that remaining stock for vantar is 2
    CU_ASSERT_EQUAL(ioopm_get_stock(wh, name2), 2);
    CU_ASSERT_EQUAL(merch->amount_in_carts, 0);

    ioopm_wh_destroy(wh);
}

static void test_checkout_2_carts()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    // inserting merch
    char *name = "skor";
    char *name2 = "vantar";
    char *desc = "fina vita skor";
    int price = 100;
    int price2 = 200;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_add_merch(wh, duplicate_string(name2), duplicate_string(desc), price2);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A3"), duplicate_string(name2), 10);

    int cart_id1 = ioopm_create_cart(wh);
    int cart_id2 = ioopm_create_cart(wh);

    // Test to decrease stock in on different shelfs
    // Takes 10 from first shelf, 2 from second
    ioopm_add_to_cart(wh, cart_id1, duplicate_string(name), 10);
    ioopm_add_to_cart(wh, cart_id2, duplicate_string(name), 10);

    ioopm_checkout(wh, cart_id1);
    ioopm_checkout(wh, cart_id2);

    // Check that cart is destroyed
    CU_ASSERT_FALSE(ioopm_hash_table_has_key_demo(wh->carts, int_elem(cart_id1)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_key_demo(wh->carts, int_elem(cart_id2)));

    // Check that all storage locations for skor is deleted from wh
    merch_lookup_t merch_lookup = ioopm_merch_lookup(wh->merches, name);
    CU_ASSERT_TRUE(merch_lookup.success); // merch should still exist
    merch_t *merch = merch_lookup.value;
    CU_ASSERT_EQUAL(ioopm_linked_list_size_demo(merch->locs), 0);
    CU_ASSERT_EQUAL(ioopm_get_stock(wh, name), 0);
    CU_ASSERT_EQUAL(merch->amount_in_carts, 0);

    ioopm_wh_destroy(wh);
}

static void test_get_locations_from_merch()
{
    ioopm_wh_t *wh = ioopm_wh_create();
    // inserting merch
    char *name = "skor";
    char *desc = "fina vita skor";
    int price = 100;
    ioopm_add_merch(wh, duplicate_string(name), duplicate_string(desc), price);
    ioopm_replenish_merch(wh, duplicate_string("A1"), duplicate_string(name), 10);
    ioopm_replenish_merch(wh, duplicate_string("A2"), duplicate_string(name), 10);

    char *non_existing_merch = "merch1";
    ioopm_list_t *merch_list = get_locations_from_merch(wh, non_existing_merch);
    CU_ASSERT_PTR_NULL(merch_list);
    ioopm_list_t *merch_list2 = get_locations_from_merch(wh, name);
    CU_ASSERT_EQUAL(ioopm_linked_list_size_demo(merch_list2), 2);

    ioopm_wh_destroy(wh);
}

int main()
{
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite my_test_suite = CU_add_suite("My awesome test suite", init_suite, clean_suite);
    if (my_test_suite == NULL)
    {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // This is where we add the test functions to our test suite.
    // For each call to CU_add_test we specify the test suite, the
    // name or description of the test, and the function that runs
    // the test in question. If you want to add another test, just
    // copy a line below and change the information
    if (
        CU_add_test(my_test_suite, "Test create and destroy", test_create_destroy_wh) == NULL ||
        CU_add_test(my_test_suite, "Test add merch", test_add_merch) == NULL ||
        CU_add_test(my_test_suite, "Test has merch", test_has_merch) == NULL ||
        CU_add_test(my_test_suite, "Test edit merch", test_edit_merch) == NULL ||
        CU_add_test(my_test_suite, "Test edit merch rewrite", test_edit_merch_rewrite) == NULL ||
        CU_add_test(my_test_suite, "Test replenish", test_replenish) == NULL ||
        CU_add_test(my_test_suite, "Test create cart", test_create_cart) == NULL ||
        CU_add_test(my_test_suite, "Test remove cart", test_remove_cart) == NULL ||
        CU_add_test(my_test_suite, "Test get stock", test_get_stock) == NULL ||
        CU_add_test(my_test_suite, "Test add to cart", test_add_to_cart) == NULL ||
        CU_add_test(my_test_suite, "Test remove from cart", test_remove_from_cart) == NULL ||
        CU_add_test(my_test_suite, "Test calculate cost", test_calculate_cost) == NULL ||
        CU_add_test(my_test_suite, "Test checkout enpty cart", test_checkout_empty_cart) == NULL ||
        CU_add_test(my_test_suite, "Test checkout", test_checkout) == NULL ||
        CU_add_test(my_test_suite, "Test checkout 2 merches", test_checkout_2_merches) == NULL ||
        CU_add_test(my_test_suite, "Test checkout 2 carts", test_checkout_2_carts) == NULL ||
        CU_add_test(my_test_suite, "Test get locations", test_get_locations_from_merch) == NULL)
    {
        // If adding any of the tests fails, we tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
    // Use CU_BRM_NORMAL to only print errors and a summary
    CU_basic_set_mode(CU_BRM_VERBOSE);

    // This is where the tests are actually run!
    CU_basic_run_tests();

    // Tear down CUnit before exiting
    CU_cleanup_registry();
    return CU_get_error();
}
