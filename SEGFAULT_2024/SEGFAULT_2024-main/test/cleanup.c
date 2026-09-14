#include <CUnit/Basic.h>
#include "tests.h"


#include "../src/refmem.h"


static void test_cleanup(void)
{
    int *int_ptr = (int*) allocate(sizeof(int), NULL);
    CU_ASSERT_PTR_NOT_NULL(int_ptr);
    retain(int_ptr);
    release(int_ptr);

    cleanup();

    CU_PASS("Check for memory leaks with Valgrind")
}



static const unit_test tests[] = {
    test_cleanup,
    NULL
};


const unit_test *get_cleanup_tests(void)
{
    return tests;
}
