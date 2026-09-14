// TODO Make unit tests for `register_type`.

#include <CUnit/Basic.h>
#include "tests.h"

// refmem library included.
#include "../../src/plugins/typemem/typemem.h"

// This is a an example test that passes.
static void example_test1(void)
{
    CU_PASS("This is an example test")
}

// This is another one.
static void example_test2(void)
{
    CU_ASSERT_TRUE(1);
}

// Add your tests into this constant `tests` array.
// Make sure it stays null terminated as C compilers
// doesn't automatically null terminate constant
// function pointer arrays.
static const unit_test tests[] = {
    example_test1,
    example_test2,
    NULL
};

// This can be left untouched.
const unit_test *get_register_type_tests(void)
{
    return tests;
}
