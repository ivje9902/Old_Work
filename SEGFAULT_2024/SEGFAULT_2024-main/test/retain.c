#include <CUnit/Basic.h>
#include "tests.h"

#include "../src/refmem.h"

// Testar att retain hanterar null utan errors
static void test_retain_null(void){
    obj *null_ptr = NULL;

    retain(null_ptr);
    CU_ASSERT_PTR_NULL(null_ptr);

}

// testar att retain inte ändrar en null pointer 
static void test_retain_null_pointer_unchanged(void){
    obj *null_ptr = NULL; 

    retain(null_ptr); 

    CU_ASSERT_PTR_NULL(null_ptr);

    CU_PASS("Retain leaves null pointer unchanged")

}
static const unit_test tests[] = {
    test_retain_null,
    test_retain_null_pointer_unchanged,
    NULL
};

// This can be left untouched.
const unit_test *get_retain_tests(void)
{
    return tests;
}
