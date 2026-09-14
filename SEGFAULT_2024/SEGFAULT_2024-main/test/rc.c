#include <CUnit/Basic.h>
#include "tests.h"


#include "../src/refmem.h"

// Börjar med att verifiera att det initiella värdet på rc vid allokering är korrekt (0). 

static void test_allocation_initial_rc_value(void){

    int *int_ptr = (int*) allocate(sizeof(int), NULL);  // Allokerar minne

    CU_ASSERT_EQUAL(rc((obj *)int_ptr), 0);             // Försäkrar att rc = 0

    deallocate((obj *)int_ptr);                         // Städar
}

// Testar rc värdet efter retain
static void test_rc_retain(void){
    int *int_ptr = (int*) allocate(sizeof(int), NULL);  // Allokerar minne

    retain((obj *)int_ptr);                             // Retain rc = 1

    CU_ASSERT_EQUAL(rc((obj *)int_ptr), 1);             // Försäkrar

    release((obj *)int_ptr);                            // Städar
}

static void test_rc_null(void){                         // Testar rc med NULL ptr
    obj *null_ptr = NULL;

    CU_ASSERT_EQUAL(rc(null_ptr), -1);

}


static const unit_test tests[] = {
    test_allocation_initial_rc_value,
    test_rc_retain,
    test_rc_null,
    NULL
};


const unit_test *get_rc_tests(void)
{
    return tests;
}
