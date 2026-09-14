#include <CUnit/Basic.h>
#include "tests.h"


#include "../src/refmem.h"


static void test_allocate_int(void){
    int *int_ptr = (int*) allocate(sizeof(int), NULL);  // Allokera minne för en integer
    CU_ASSERT_PTR_NOT_NULL(int_ptr);                    // Kollar om allokering lyckats

    *int_ptr = 1337;                                    // Provar att lägga in en siffra
    CU_ASSERT_EQUAL(*int_ptr, 1337);                      // Försäkrar att det fungerat 

    deallocate(int_ptr);

    CU_PASS("Memory allocated and deallocated");
}

typedef struct{
    int array[10];
    double value;
    char z; 
} array_struct;

// För att visa att allocate fungerar på mer komplexa strukturer/olika typer av data
static void test_allocate_struct_with_array(void){
    
    array_struct *struct_ptr = (array_struct *) allocate(sizeof(array_struct), NULL);
    CU_ASSERT_PTR_NOT_NULL(struct_ptr);          

    // Tilldelar värden till arrayen och doublen i array_struct
    for (int i = 0; i < 10; ++i){
        struct_ptr->array[i] = i * 2;   // Tilldelar jämna nummer till arrayen
    }
    struct_ptr->value = 1.234;

    for (int i = 0; i < 10; ++i) {      // Försäkrar att värdena i arrayen är korrekt
        CU_ASSERT_EQUAL(struct_ptr->array[i], i * 2);
    }
    CU_ASSERT_DOUBLE_EQUAL(struct_ptr->value, 1.234, 0.001); 

    deallocate((obj *)struct_ptr);      

    CU_PASS("Struct including array succesfully allocated and deallocated");
}


static const unit_test tests[] = {
    test_allocate_int,
    test_allocate_struct_with_array,
    NULL
};


const unit_test *get_allocate_tests(void)
{
    return tests;
}
