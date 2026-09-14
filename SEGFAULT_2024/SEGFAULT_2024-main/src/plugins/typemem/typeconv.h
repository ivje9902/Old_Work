#pragma once

#define read_mem(type) \
    __read_mem(#type)

// Converts to function call to allocate from a registered type
#define allocate_from_type(type) \
    __allocate_from_type(#type)

#define register_type0(type) \
    "Error, needs be on form register_type(type, f1, ...)"

// Converts to function call for types with 1 pointer
#define register_type1(type, f1) \
    __register_type(#type, sizeof(type), 1, offsetof(type, f1))

// Converts to function call for types with 2 pointers
#define register_type2(type, f1, f2) \
    __register_type(#type, sizeof(type), 2, offsetof(type, f1), offsetof(type, f2))

// Converts to function call for types with 3 pointers
#define register_type3(type, f1, f2, f3) \
    __register_type(#type, sizeof(type), 3, offsetof(type, f1), offsetof(type, f2), offsetof(type, f3))

// Converts to function call for types with 4 pointers
#define register_type4(type, f1, f2, f3, f4) \
    __register_type(#type, sizeof(type), 4, offsetof(type, f1), offsetof(type, f2), offsetof(type, f3), offsetof(type, f4))

// Converts to function call for types with 5 pointers
#define register_type5(type, f1, f2, f3, f4, f5) \
    __register_type(#type, sizeof(type), 5, offsetof(type, f1), offsetof(type, f2), offsetof(type, f3), offsetof(type, f4), offsetof(type, f5))

// Converts to function call for types with 6 pointers
#define register_type6(type, f1, f2, f3, f4, f5, f6) \
    __register_type(#type, sizeof(type), 6, offsetof(type, f1), offsetof(type, f2), offsetof(type, f3), offsetof(type, f4), offsetof(type, f5), offsetof(type, f6))

// Converts to function call for types with 7 pointers
#define register_type7(type, f1, f2, f3, f4, f5, f6, f7) \
    __register_type(#type, sizeof(type), 7, offsetof(type, f1), offsetof(type, f2), offsetof(type, f3), offsetof(type, f4), offsetof(type, f5), offsetof(type, f6), offsetof(type, f7))

// Converts to function call for types with 8 pointers
#define register_type8(type, f1, f2, f3, f4, f5, f6, f7, f8) \
    __register_type(#type, sizeof(type), 8, offsetof(type, f1), offsetof(type, f2), offsetof(type, f3), offsetof(type, f4), offsetof(type, f5), offsetof(type, f6), offsetof(type, f7), offsetof(type, f8))

// Converts to function call for types with 9 pointers
#define register_type9(type, f1, f2, f3, f4, f5, f6, f7, f8, f9) \
    __register_type(#type, sizeof(type), 9, offsetof(type, f1), offsetof(type, f2), offsetof(type, f3), offsetof(type, f4), offsetof(type, f5), offsetof(type, f6), offsetof(type, f7), offsetof(type, f8), offsetof(type, f9))
