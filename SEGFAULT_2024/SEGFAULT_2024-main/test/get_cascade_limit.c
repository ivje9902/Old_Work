// TODO Make unit tests for `get_cascade_limit`.

#include <CUnit/Basic.h>
#include "tests.h"


#include "../src/refmem.h"

static void example_test1(void)
{
    CU_PASS("This is an example test")
}


static void example_test2(void)
{
    CU_ASSERT_TRUE(1);
}


static const unit_test tests[] = {
    example_test1,
    example_test2,
    NULL
};


const unit_test *get_get_cascade_limit_tests(void)
{
    return tests;
}
