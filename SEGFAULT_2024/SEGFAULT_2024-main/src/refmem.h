#pragma once

#include <stddef.h>

typedef void obj;
typedef void (*function1_t)(obj *);

/**
 * @brief Retains the reference count of an object.
 *
 * This function increments the reference count of the given object.
 *
 * @param object A pointer to the object.
 */
void retain(obj *);

/**
 * @brief Releases the reference count of an object.
 *
 * This function decrements the reference count of the given object.
 *
 * @param object A pointer to the object.
 */
void release(obj *);

/**
 * @brief Returns the reference count of an object.
 *
 * This function returns the current reference count of the given object.
 *
 * @param object A pointer to the object.
 * @return The reference count of the object.
 */
size_t rc(obj *);

/**
 * @brief Allocates memory for an object.
 *
 * This function allocates memory for an object of the specified size and associates
 * a destructor function for cleanup.
 *
 * @param bytes The size of the memory to allocate.
 * @param destructor The destructor function to be called when the object is deallocated possily NULL.
 * @return A pointer to the allocated object.
 */
obj *allocate(size_t bytes, function1_t destructor);

/**
 * @brief Allocates memory for an array of objects.
 *
 * This function allocates memory for an array of objects with the specified number of elements,
 * each having the specified element size, and associates a destructor function for cleanup.
 *
 * @param elements The number of elements in the array.
 * @param elem_size The size of each element in the array.
 * @param destructor The destructor function to be called when the array is deallocated possily NULL.
 * @return A pointer to the allocated array.
 */
obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor);

/**
 * @brief Deallocates memory for an object or array.
 *
 * This function deallocates the memory associated with the given object or array.
 *
 * @param object A pointer to the object or array to deallocate.
 *
 * @pre Can not deallocate an object with rc(o) != 0
 */
void deallocate(obj *);

/**
 * @brief Sets the cascade limit for reference counting.
 *
 * This function sets the cascade limit for reference counting. The cascade limit determines
 * the maximum number of times release operations cascade through referenced objects.
 *
 * @param limit The cascade limit to set.
 * @def Default value = 10
 */
void set_cascade_limit(size_t);

/**
 * @brief Gets the current cascade limit for reference counting.
 *
 * This function returns the current cascade limit.
 *
 * @return The current cascade limit.
 */
size_t get_cascade_limit(void);

/**
 * @brief Performs cleanup operations.
 *
 * This function performs any necessary cleanup operations, such as releasing resources thar has no references.
 */
void cleanup(void);

/**
 * @brief Shuts down the reference counting system.
 *
 * This function shuts down the reference counting system, releasing any remaining resources.
 */
void shutdown(void);
