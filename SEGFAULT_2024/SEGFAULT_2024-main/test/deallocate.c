#include <CUnit/Basic.h>
#include "tests.h"

#include "../src/refmem.h"

// Testar deallocate med 0 referens count
static void test_deallocate_zero_count(void)
{
    obj *object = allocate(sizeof(int), NULL); // Allokera nytt objekt
    CU_ASSERT_EQUAL(rc(object), 0);            // Försäkrar oss att referns counten är 0
    deallocate(object);                        // Enligt vår implementation ska nu objektet deallokeras
    CU_PASS("deallocate completed without errors, ensure there are no memory leaks using valgrind"); 
    
}


static const unit_test tests[] = {
    test_deallocate_zero_count,
    NULL
};

const unit_test *get_deallocate_tests(void)
{
    return tests;
}
