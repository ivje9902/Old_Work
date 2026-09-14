#include "meta.h"

// Transforms metadata to an obj * <-> (void *)
// returning a pointer to the memory after the meta_data
// from the figure above: given a pointer p returns q (field 3)
void *meta_to_obj(meta_data *meta)
{
	return meta + 1; // C (magically) multiplies the offset by the size if the meta data
}

// Transforms an object to meta data
// returning a pointer to the memory before the object data
// from the figure above: given a pointer q returns p (field 1 & 2)
meta_data *obj_to_meta(void *object)
{
	return (meta_data *)object - 1;
}
