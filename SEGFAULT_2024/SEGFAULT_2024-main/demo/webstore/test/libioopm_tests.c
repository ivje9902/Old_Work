#include <string.h>
#include <CUnit/Basic.h>
#include "../libwebstore/hash_table.h"
#include "../libwebstore/linked_list.h"
#include "../libwebstore/private.h"
#include "../libwebstore/iterator.h"
#include <string.h>

#define int_elem(x) \
    (elem_t) { .int_value = x }
#define ptr_elem(x) \
    (elem_t) { .pointer_value = (x) }

#define ioopm_int_ll_append(ll, s) \
    ioopm_linked_list_append_demo(ll, int_elem(s))

#define ioopm_int_ll_prepend(ll, s) \
    ioopm_linked_list_prepend_demo(ll, int_elem(s))

#define ioopm_int_str_ht_insert(ht, i, s) \
    ioopm_hash_table_insert_demo(ht, int_elem(i), ptr_elem(s))

static bool compare_int_elements(elem_t a, elem_t b)
{
    return b.int_value == a.int_value;
}

static bool cmp_int_elements(elem_t a, elem_t b)
{
    return b.int_value == a.int_value;
}

static bool cmp_string_elements(elem_t a, elem_t b)
{
    return !strcmp(a.pointer_value, b.pointer_value);
}

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

static void test_create_destroy()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    CU_ASSERT_PTR_NOT_NULL(ht);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_insert_once1()
{
    int key = 1;
    char *value = "test";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_lookup_demo(ht, int_elem(key))));
    ioopm_int_str_ht_insert(ht, key, value);
    // CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, int_elem(key)).value, ptr_elem(value));
    ioopm_hash_table_destroy_demo(ht);
}
static void test_insert_once2()
{
    int key = 1;
    int key2 = 1;
    char *value = "test";
    char *value2 = "hej";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_lookup_demo(ht, int_elem(key))));
    ioopm_int_str_ht_insert(ht, key, value);
    CU_ASSERT_EQUAL(ioopm_hash_table_lookup_demo(ht, int_elem(key)).value.pointer_value, value);
    ioopm_int_str_ht_insert(ht, key2, value2);
    CU_ASSERT_EQUAL(ioopm_hash_table_lookup_demo(ht, int_elem(key2)).value.pointer_value, value2);

    ioopm_hash_table_destroy_demo(ht);
}

static void test_insert_once3()
{
    int key = -1;
    char *value = NULL;
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_lookup_demo(ht, int_elem(key))));
    ioopm_int_str_ht_insert(ht, key, value);
    CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_lookup_demo(ht, int_elem(key))));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_lookup_empty()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    for (int i = 0; i < 18; ++i) /// 18 is a bit magical
    {
        CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_lookup_demo(ht, int_elem(i))));
    }
    CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_lookup_demo(ht, int_elem(-1))));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_remove_insert()
{
    int key = 4;
    char *value = "test";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key, value);
    CU_ASSERT_EQUAL(ioopm_hash_table_lookup_demo(ht, int_elem(key)).value.pointer_value, value);
    option_t rmv = ioopm_hash_table_remove_demo(ht, int_elem(key));
    CU_ASSERT_EQUAL(rmv.value.pointer_value, value);
    CU_ASSERT_TRUE(Successful(rmv));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_remove_not_in_table()
{
    int key = 4;
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_remove_demo(ht, int_elem(key))));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_remove_invalid_key()
{
    int key = -1;
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    CU_ASSERT_TRUE(Unsuccessful(ioopm_hash_table_remove_demo(ht, int_elem(key))));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_size()
{
    int key = 4;
    char *value = "test";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key, value);
    int size = ioopm_hash_table_size_demo(ht);
    CU_ASSERT_EQUAL(size, 1);
    ioopm_hash_table_clear_demo(ht);
    size = ioopm_hash_table_size_demo(ht);
    CU_ASSERT_EQUAL(size, 0);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_clear()
{
    int key = 4;
    char *value = "test";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key, value);
    ioopm_hash_table_clear_demo(ht);
    CU_ASSERT_TRUE(ioopm_hash_table_is_empty_demo(ht));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_keys()
{
    int keys[5] = {3, 10, 42, 0, 99};
    bool found[5] = {false};
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    for (int i = 0; i < 5; i++)
    {
        ioopm_int_str_ht_insert(ht, keys[i], "foo");
    }
    ioopm_list_t *keysfound = ioopm_hash_table_keys_demo(ht);
    ioopm_list_iterator_t *iter = ioopm_list_iterator_demo(keysfound);
    for (int i = 0; i < 5; i++)
    {
        while (ioopm_iterator_has_next_demo(iter))
        {
            if (keys[i] == ioopm_iterator_next_demo(iter).int_value)
            {
                found[i] = true;
            }
        }
        ioopm_iterator_reset_demo(iter);
        if (!found[i])
        {
            CU_FAIL("Found a key that was never inserted");
        }
    }
    for (int i = 0; i < 5; i++)
    {
        CU_ASSERT_TRUE(found[i]);
    }
    ioopm_linked_list_destroy_demo(keysfound);
    ioopm_iterator_destroy_demo(iter);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_values()
{
    int keys[5] = {3, 10, 42, 0, 99};
    char *strings[5] = {"three", "ten", "fortytwo", "zero", "ninetynine"};
    bool found[5] = {false};
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    for (int i = 0; i < 5; i++)
    {
        ioopm_int_str_ht_insert(ht, keys[i], strings[i]);
    }
    ioopm_list_t *keysfound = ioopm_hash_table_keys_demo(ht);
    ioopm_list_iterator_t *iter_keys = ioopm_list_iterator_demo(keysfound);
    ioopm_list_t *valuesfound = ioopm_hash_table_values_demo(ht);
    ioopm_list_iterator_t *iter_values = ioopm_list_iterator_demo(valuesfound);
    while (ioopm_iterator_has_next_demo(iter_keys) && ioopm_iterator_has_next_demo(iter_values))
    {
        for (int i = 0; i < 5; i++)
        {
            if (keys[i] == ioopm_iterator_current_demo(iter_keys).int_value && !strcmp(strings[i], ioopm_iterator_current_demo(iter_values).pointer_value))
            {
                found[i] = true;
            }
        }
        ioopm_iterator_next_demo(iter_keys);
        ioopm_iterator_next_demo(iter_values);
    }
    for (int i = 0; i < 5; i++)
    {
        CU_ASSERT_TRUE(found[i]);
    }
    ioopm_linked_list_destroy_demo(valuesfound);
    ioopm_linked_list_destroy_demo(keysfound);
    ioopm_iterator_destroy_demo(iter_keys);
    ioopm_iterator_destroy_demo(iter_values);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_has_key1()
{
    int key = 18;
    char *value = "test";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key, value);
    CU_ASSERT_TRUE(ioopm_hash_table_has_key_demo(ht, int_elem(key)));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_has_key2()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    CU_ASSERT_FALSE(ioopm_hash_table_has_key_demo(ht, int_elem(12)));
    ioopm_hash_table_destroy_demo(ht);
}

static void test_has_value1()
{
    int key = 18;
    char *value = "test";
    char *cpy = strdup(value);
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key, value);
    CU_ASSERT_TRUE(ioopm_hash_table_has_value_demo(ht, ptr_elem(value)));
    CU_ASSERT_TRUE(ioopm_hash_table_has_value_demo(ht, ptr_elem(cpy)));
    free(cpy);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_has_value2()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, 2, "hej");
    CU_ASSERT_FALSE(ioopm_hash_table_has_value_demo(ht, ptr_elem("test")));
    ioopm_hash_table_destroy_demo(ht);
}

static bool string_equiv(__attribute_maybe_unused__ elem_t value_ignored, elem_t value, void *x)
{
    return !strcmp(value.pointer_value, x);
}

static void test_has_any()
{
    int key1 = 18;
    int key2 = 1;
    int key3 = 7;
    char *value1 = "test";
    char *value2 = "new";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key1, value1);
    ioopm_int_str_ht_insert(ht, key2, value1);
    bool result = ioopm_hash_table_any_demo(ht, string_equiv, value2);
    CU_ASSERT_FALSE(result);
    ioopm_int_str_ht_insert(ht, key3, value2);
    result = ioopm_hash_table_any_demo(ht, string_equiv, value2);
    CU_ASSERT_TRUE(result);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_has_all1()
{
    int key1 = 18;
    int key2 = 1;
    int key3 = 7;
    char *value = "test";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key1, value);
    ioopm_int_str_ht_insert(ht, key2, value);
    ioopm_int_str_ht_insert(ht, key3, value);
    bool result = ioopm_hash_table_all_demo(ht, string_equiv, value);
    CU_ASSERT_TRUE(result);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_has_all2()
{
    int key1 = 18;
    int key2 = 1;
    int key3 = 7;
    char *value = "test";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key1, value);
    ioopm_int_str_ht_insert(ht, key2, value);
    ioopm_int_str_ht_insert(ht, key3, "hej");
    bool result = ioopm_hash_table_all_demo(ht, string_equiv, value);
    CU_ASSERT_FALSE(result);
    ioopm_hash_table_destroy_demo(ht);
}

static void replace_value(__attribute_maybe_unused__ elem_t value_ignored, elem_t *value, void *x)
{
    *value = *((elem_t *)x);
}

static void test_apply_to_all()
{
    int key1 = 18;
    int key2 = 1;
    int key3 = 7;
    char *value = "foobarbaz";
    ioopm_hash_table_t *ht = ioopm_hash_table_create_demo(NULL, cmp_int_elements, cmp_string_elements);
    ioopm_int_str_ht_insert(ht, key1, "foo");
    ioopm_int_str_ht_insert(ht, key2, "bar");
    ioopm_int_str_ht_insert(ht, key3, "baz");
    bool result = ioopm_hash_table_all_demo(ht, string_equiv, value);
    CU_ASSERT_FALSE(result);
    ioopm_hash_table_apply_to_all_demo(ht, replace_value, &value);
    result = ioopm_hash_table_all_demo(ht, string_equiv, value);
    CU_ASSERT_TRUE(result);
    ioopm_hash_table_destroy_demo(ht);
}

static void test_ll_create_destroy()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    CU_ASSERT_PTR_NOT_NULL(ll);
    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_append()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_append(ll, 10);
    ioopm_int_ll_append(ll, 5);
    CU_ASSERT_EQUAL(ll->first->next->element.int_value, 10);
    CU_ASSERT_EQUAL(ll->first->next->next->element.int_value, 5);
    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_preppend()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 8);
    CU_ASSERT_EQUAL(ll->first->next->element.int_value, 8);
    CU_ASSERT_EQUAL(ll->first->next->next->element.int_value, 5);
    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_size()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 8);
    ioopm_int_ll_prepend(ll, 10);
    CU_ASSERT_EQUAL(ioopm_linked_list_size_demo(ll), 3);
    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_remove()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 8);
    ioopm_int_ll_prepend(ll, 10);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove_demo(ll, 0).int_value, 10);
    CU_ASSERT_EQUAL(ioopm_linked_list_size_demo(ll), 2);
    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_get()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 8);
    ioopm_int_ll_prepend(ll, 10);
    CU_ASSERT_EQUAL(ioopm_linked_list_get_demo(ll, 2)->int_value, 5);
    CU_ASSERT_EQUAL(ioopm_linked_list_get_demo(ll, 1)->int_value, 8);
    CU_ASSERT_EQUAL(ioopm_linked_list_get_demo(ll, 0)->int_value, 10);

    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_contains()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    CU_ASSERT_TRUE(ioopm_linked_list_contains_demo(ll, int_elem(5)));
    CU_ASSERT_FALSE(ioopm_linked_list_contains_demo(ll, int_elem(10)));
    ioopm_int_ll_prepend(ll, 10);
    CU_ASSERT_TRUE(ioopm_linked_list_contains_demo(ll, int_elem(10)));

    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_is_empty()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    CU_ASSERT_TRUE(ioopm_linked_list_is_empty_demo(ll));
    ioopm_int_ll_prepend(ll, 5);
    CU_ASSERT_FALSE(ioopm_linked_list_is_empty_demo(ll));
    ioopm_linked_list_destroy_demo(ll);
}

static bool elemet_equiv(__attribute_maybe_unused__ elem_t key, elem_t element, void *x)
{
    elem_t *ptr = x;
    elem_t val = *ptr;
    return element.int_value == val.int_value;
}

static void test_ll_any()
{
    int ref = 10;
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 5);
    CU_ASSERT_FALSE(ioopm_linked_list_any_demo(ll, elemet_equiv, &ref));
    ioopm_int_ll_prepend(ll, 10);
    CU_ASSERT_TRUE(ioopm_linked_list_any_demo(ll, elemet_equiv, &ref));

    ioopm_linked_list_destroy_demo(ll);
}

static void test_ll_all()
{
    elem_t ref = int_elem(5);
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 5);
    CU_ASSERT_TRUE(ioopm_linked_list_all_demo(ll, elemet_equiv, &ref));
    ioopm_int_ll_prepend(ll, 10);
    CU_ASSERT_FALSE(ioopm_linked_list_all_demo(ll, elemet_equiv, &ref));

    ioopm_linked_list_destroy_demo(ll);
}

static void elemet_replace(__attribute_maybe_unused__ elem_t ignored, elem_t *element, void *x)
{
    *element = *(elem_t *)x;
}

static void test_ll_apply_all()
{
    int ref = 5;
    int new = 10;
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 5);
    ioopm_int_ll_prepend(ll, 5);
    CU_ASSERT_TRUE(ioopm_linked_list_all_demo(ll, elemet_equiv, &ref));
    ioopm_linked_list_apply_to_all_demo(ll, elemet_replace, &new);
    CU_ASSERT_TRUE(ioopm_linked_list_all_demo(ll, elemet_equiv, &new));

    ioopm_linked_list_destroy_demo(ll);
}

static void test_iterator_create()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_list_iterator_t *iterator = ioopm_list_iterator_demo(ll);
    CU_ASSERT_PTR_NOT_NULL(iterator);
    ioopm_linked_list_destroy_demo(ll);
    ioopm_iterator_destroy_demo(iterator);
}

static void test_iterator_next()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 3);
    ioopm_int_ll_prepend(ll, 2);
    ioopm_int_ll_prepend(ll, 1);
    ioopm_list_iterator_t *iterator = ioopm_list_iterator_demo(ll);
    ioopm_iterator_next_demo(iterator);
    int nextint2 = iterator->current->element.int_value;
    CU_ASSERT_PTR_NOT_NULL(iterator);
    CU_ASSERT_EQUAL(nextint2, 1);
    ioopm_linked_list_destroy_demo(ll);
    ioopm_iterator_destroy_demo(iterator);
}

static void test_iterator_current()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 3);
    ioopm_int_ll_prepend(ll, 2);
    ioopm_int_ll_prepend(ll, 1);
    ioopm_list_iterator_t *iterator = ioopm_list_iterator_demo(ll);
    ioopm_iterator_next_demo(iterator);
    int currentint = ioopm_iterator_current_demo(iterator).int_value;
    CU_ASSERT_PTR_NOT_NULL(iterator);
    CU_ASSERT_EQUAL(currentint, 2);
    ioopm_linked_list_destroy_demo(ll);
    ioopm_iterator_destroy_demo(iterator);
}

static void test_iterator_reset()
{
    ioopm_list_t *ll = ioopm_linked_list_create_demo(compare_int_elements);
    ioopm_int_ll_prepend(ll, 3);
    ioopm_int_ll_prepend(ll, 2);
    ioopm_int_ll_prepend(ll, 1);
    ioopm_list_iterator_t *iterator = ioopm_list_iterator_demo(ll);
    ioopm_iterator_next_demo(iterator);
    ioopm_iterator_next_demo(iterator);
    ioopm_iterator_reset_demo(iterator);
    int currentint = ioopm_iterator_current_demo(iterator).int_value;
    CU_ASSERT_EQUAL(currentint, 1);
    ioopm_linked_list_destroy_demo(ll);
    ioopm_iterator_destroy_demo(iterator);
}

int main()
{
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite my_test_suite = CU_add_suite("Hash table tests", init_suite, clean_suite);
    if (my_test_suite == NULL)
    {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite my_test_suite1 = CU_add_suite("linked list tests", init_suite, clean_suite);
    if (my_test_suite1 == NULL)
    {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_pSuite my_test_suite2 = CU_add_suite("iterator tests", init_suite, clean_suite);
    if (my_test_suite2 == NULL)
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
        CU_add_test(my_test_suite, "create destroy", test_create_destroy) == NULL ||
        CU_add_test(my_test_suite, "insert1", test_insert_once1) == NULL ||
        CU_add_test(my_test_suite, "insert2", test_insert_once2) == NULL ||
        CU_add_test(my_test_suite, "insert3", test_insert_once3) == NULL ||
        CU_add_test(my_test_suite, "test lookup empty", test_lookup_empty) == NULL ||
        CU_add_test(my_test_suite, "test remove insert", test_remove_insert) == NULL ||
        CU_add_test(my_test_suite, "test remove", test_remove_not_in_table) == NULL ||
        CU_add_test(my_test_suite, "test remove", test_remove_invalid_key) == NULL ||
        CU_add_test(my_test_suite, "test size", test_size) == NULL ||
        CU_add_test(my_test_suite, "test clear", test_clear) == NULL ||
        CU_add_test(my_test_suite, "test keys", test_keys) == NULL ||
        CU_add_test(my_test_suite, "test values", test_values) == NULL ||
        CU_add_test(my_test_suite, "test has key1", test_has_key1) == NULL ||
        CU_add_test(my_test_suite, "test has key2", test_has_key2) == NULL ||
        CU_add_test(my_test_suite, "test has value1", test_has_value1) == NULL ||
        CU_add_test(my_test_suite, "test has value2", test_has_value2) == NULL ||
        CU_add_test(my_test_suite, "test has any", test_has_any) == NULL ||
        CU_add_test(my_test_suite, "test has all1", test_has_all1) == NULL ||
        CU_add_test(my_test_suite, "test has all2", test_has_all2) == NULL ||
        CU_add_test(my_test_suite, "test apply to all", test_apply_to_all) == NULL ||
        CU_add_test(my_test_suite1, "ll create destroy", test_ll_create_destroy) == NULL ||
        CU_add_test(my_test_suite1, "ll append", test_ll_append) == NULL ||
        CU_add_test(my_test_suite1, "ll preppend", test_ll_preppend) == NULL ||
        CU_add_test(my_test_suite1, "ll size", test_ll_size) == NULL ||
        CU_add_test(my_test_suite1, "ll remove", test_ll_remove) == NULL ||
        CU_add_test(my_test_suite1, "ll get", test_ll_get) == NULL ||
        CU_add_test(my_test_suite1, "ll contains", test_ll_contains) == NULL ||
        CU_add_test(my_test_suite1, "ll is empty", test_ll_is_empty) == NULL ||
        CU_add_test(my_test_suite1, "ll any", test_ll_any) == NULL ||
        CU_add_test(my_test_suite1, "ll all", test_ll_all) == NULL ||
        CU_add_test(my_test_suite1, "ll apply_all", test_ll_apply_all) == NULL ||
        CU_add_test(my_test_suite2, "iterator create", test_iterator_create) == NULL ||
        CU_add_test(my_test_suite2, "iterator current", test_iterator_current) == NULL ||
        CU_add_test(my_test_suite2, "iterator next", test_iterator_next) == NULL ||
        CU_add_test(my_test_suite2, "iterator reset", test_iterator_reset) == NULL)
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
