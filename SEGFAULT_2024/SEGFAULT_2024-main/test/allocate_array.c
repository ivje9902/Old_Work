#include <CUnit/Basic.h>
#include "tests.h"


#include "../src/refmem.h"


static void test_allocate_array(void){
    //Testar funktionaliteten i ett normalfall (array med storlek 10)
    int *arr = (int *) allocate_array(10, sizeof(int), NULL);

    CU_ASSERT_PTR_NOT_NULL(arr); // Försäkrar oss om att arr inte är NULL.


    for (int i = 0; i < 10; i++) { // Försäkrar oss om att alla element är 0 initialt.
        CU_ASSERT(arr[i] == 0);
    }

    deallocate(arr); //rensar
}


static void test_allocate_zero_array(void){
    // Testar allokering av en array med noll element.
    // Enligt vår implementation ska det returnas ett minnesblock av storlek 0.

    int *arr = (int *) allocate_array(0, sizeof(int), NULL); // Allokerar array med 0 element

    // Kontrollerar att allocate_array inte returnerar NULL trots att antalet element är 0.
    // Detta betyder att en allokeringen lyckats med ett minnesblock av storlek 0.
    CU_ASSERT_PTR_NOT_NULL(arr);

    deallocate(arr);
}


static const unit_test tests[] = {
    test_allocate_array,
    test_allocate_zero_array,
    NULL
};


const unit_test *get_allocate_array_tests(void)
{
    return tests;
}
