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
 * `allocate` unit test functions
 */
const unit_test *get_allocate_tests(void);

/**
 * @return null terminated array of function pointers to
 * `allocate_array` unit test functions
 */
const unit_test *get_allocate_array_tests(void);

/**
 * @return null terminated array of function pointers to
 * `cleanup` unit test functions
 */
const unit_test *get_cleanup_tests(void);

/**
 * @return null terminated array of function pointers to
 * `deallocate` unit test functions
 */
const unit_test *get_deallocate_tests(void);

/**
 * @return null terminated array of function pointers to
 * `get_cascade_limit` unit test functions
 */
const unit_test *get_get_cascade_limit_tests(void);

/**
 * @return null terminated array of function pointers to
 * `rc` unit test functions
 */
const unit_test *get_rc_tests(void);

/**
 * @return null terminated array of function pointers to
 * `release` unit test functions
 */
const unit_test *get_release_tests(void);

/**
 * @return null terminated array of function pointers to
 * `retain` unit test functions
 */
const unit_test *get_retain_tests(void);

/**
 * @return null terminated array of function pointers to
 * `set_cascade_limit` unit test functions
 */
const unit_test *get_set_cascade_limit_tests(void);

/**
 * @return null terminated array of function pointers to
 * `shutdown` unit test functions
 */
const unit_test *get_shutdown_tests(void);
