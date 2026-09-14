#pragma once

// Initial struct for object with reference counter field
// Might need further additions!! /J

/* Figure 1
+---+---+---------+
| 1 | 2 |    3    |
+---+---+---------+
  ^       ^
  p       q
*/

typedef void (*destructor_fun)(void *);

typedef struct                              //  +---+---+-------------+
{                                           //  | 1 | 2 | 3 |    4    |
  unsigned int ref_count; // field 1        //  +---+---+-------------+
  destructor_fun destroy; // field 2        //  ^           ^
  char *type;             // field 3        //  meta_data   obj
} meta_data;

/**
 * @brief Transforms metadata to an object pointer.
 *
 * This function takes a pointer to metadata (meta_data) and returns a pointer to the
 * memory after the metadata, effectively converting metadata to an object pointer.
 *
 * @param meta A pointer to the metadata structure.
 * @return A pointer to the memory after the metadata, pointing to the object data.
 */
void *meta_to_obj(meta_data *meta);

/**
 * @brief Transforms an object pointer to metadata.
 *
 * This function takes a pointer to object data and returns a pointer to the
 * memory before the object data, effectively converting an object pointer to metadata.
 *
 * @param object A pointer to the object data.
 * @return A pointer to the memory before the object data, pointing to the metadata structure.
 */
meta_data *obj_to_meta(void *object);
