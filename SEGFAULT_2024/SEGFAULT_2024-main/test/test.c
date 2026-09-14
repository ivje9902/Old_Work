#include <CUnit/Basic.h>

#include "tests.h"

static int suite_init(void)
{
    return 0;
}

static int suite_clean(void)
{
    return 0;
}

int main(void)
{
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    const CU_pSuite suite =
        CU_add_suite("IOOPM refmem library test suite", suite_init,
                     suite_clean);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    int fail_to_add_tests = 0;

    // Below are loops that iterates over all returned test
    // function pointers and adds them to the test suite.

    for (const unit_test * t = get_allocate_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_allocate_array_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_cleanup_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_deallocate_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_get_cascade_limit_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_rc_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_release_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_retain_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_set_cascade_limit_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    for (const unit_test * t = get_shutdown_tests(); *t != NULL; t++) {
        if (CU_add_test(suite, "test", *t) == NULL) {
            fail_to_add_tests = 1;
        }
    }

    if (fail_to_add_tests) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    return CU_get_error();
}
