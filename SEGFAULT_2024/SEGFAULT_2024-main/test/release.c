#include <CUnit/Basic.h>
#include "tests.h"

#include "../src/refmem.h"

static int destructor_used = 0;

static void custom_destructor(obj *o) {
    (void)o;
    destructor_used = 1;           // berättar att destructor är använd
}

static void test_release_triggers_deallocation(void){
    destructor_used = 0;                                            
    int *int_ptr = (int *) allocate(sizeof(int), custom_destructor);    // kallar på allocate med custom_destructor
    CU_ASSERT_PTR_NOT_NULL(int_ptr);                                 

    retain((obj *)int_ptr);                     // rc = 1   
    release((obj *)int_ptr);                    // rc = 0 bör kalla på custom_destructor, därmed destructor_used = 1

    CU_ASSERT_TRUE(destructor_used);            // Kollar att destruktorn har kallats, därmed friat minnet
}

static void test_release_null(void){
    obj *null_ptr = NULL;

    release(null_ptr);
    CU_ASSERT_PTR_NULL(null_ptr);

}

// Testar både release och retain
static void test_release_and_retain(void)
{
    int *int_ptr = (int *) allocate(sizeof(int), NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(int_ptr);

    CU_ASSERT_EQUAL(rc((obj *)int_ptr), 0);  // initial rc ska vara 0

    release((obj *)int_ptr);
    CU_ASSERT_EQUAL(rc((obj *)int_ptr), 0);

    retain((obj *)int_ptr);
    CU_ASSERT_EQUAL(rc((obj *)int_ptr), 1);  // rc ska vara 1

    retain((obj *)int_ptr);
    CU_ASSERT_EQUAL(rc((obj *)int_ptr), 2);  // retain igen rc ska vara 2

    release((obj *)int_ptr); 
    CU_ASSERT_EQUAL(rc((obj *)int_ptr), 1); // rc ska vara 1 efter release

    release((obj *)int_ptr); 

    CU_PASS("Release correctly handles reference count");
}


static const unit_test tests[] = {
    test_release_and_retain,
    test_release_triggers_deallocation,
    test_release_null,
    NULL
};

const unit_test *get_release_tests(void)
{
    return tests;
}
