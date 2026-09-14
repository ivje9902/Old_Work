#pragma once

/*
 * Definition for CUnit unit test function pointer
 */
typedef void (*unit_test)(void);

/*
 * These following functions each returns a null terminated
 * array of function pointers to unit test functions.
 */

/**
 * @return null terminated array of function pointers to
 * `register_type` unit test functions
 */
const unit_test *get_register_type_tests(void);
