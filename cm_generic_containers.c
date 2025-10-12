#ifndef CM_GENERIC_CONTAINERS_C
#define CM_GENERIC_CONTAINERS_C

#include "cm_string.c"
#include "cm_types.h"

#define _VRELEASE\
  if (_a->arr){\
  _a->capacity = 0;\
  _a->size     = 0;\
  _a->arr      = NULL;\
  heap_free_dz(_a->arr);\
  }\
  return true;

#define _VINIT\
  heap_alloc_dz(sizeof(_b.arr) * max, _b.arr);\
  _a->capacity = max;\
  _a->size = 0;\
  _a->arr = _b.arr;\
  return true;

#define _VADD\
  if (_a->size >= _a->capacity) {\
    return false;}\
  _a->arr[_a->size] = _s;\
  _a->size++;\
  return true;

#define CM_STRUCT typedef struct

#define V_STRUCT(T) CM_STRUCT T ## _v { T* arr; u32 size; u32 capacity;} T ## _v

#define V_ADD_FN(T) static bool T ## _vadd( T ## _v *_a, T _s) { _VADD }

#define V_INIT_FN(T) static bool T ## _vinit(T ## _v *_a, u32 max) {T ## _v _b = {0}; _VINIT }

#define V_RELEASE_FN(T) static bool T ## _vrelease(T ## _v *_a) { _VRELEASE }

#define V_FULL(T) V_STRUCT(T); V_ADD_FN(T); V_INIT_FN(T); V_RELEASE_FN(T);

V_FULL(i8);
V_FULL(i16)
V_FULL(i32)
V_FULL(i64)
V_FULL(u8);
V_FULL(u16)
V_FULL(u32)
V_FULL(u64)
V_FULL(S8);

[[deprecated]] bool
generic_vadd(MU void* t, MU void* x)
{
  return true;
}

[[deprecated]] bool
generic_vinit(MU void* t, MU void* x)
{
  return true;
}

[[deprecated]] bool
generic_vrelease(MU void* t)
{
  return true;
}


#define v_init(T, X) _Generic((T) ,\
    S8_v*:        S8_vinit,\
    i8_v*:        i8_vinit,\
    i16_v*:       i16_vinit,\
    i32_v*:       i32_vinit,\
    i64_v*:       i64_vinit,\
    u8_v*:        u8_vinit,\
    u16_v*:       u16_vinit,\
    u32_v*:       u32_vinit,\
    u64_v*:       u64_vinit,\
    default:      (T) \
)(T, X)

#define v_release(T) _Generic((T) ,\
    S8_v*:         S8_vrelease     ,\
    i8_v*:         i8_vrelease     ,\
    i16_v*:        i16_vrelease    ,\
    i32_v*:        i32_vrelease    ,\
    i64_v*:        i64_vrelease    ,\
    u8_v*:         u8_vrelease     ,\
    u16_v*:        u16_vrelease    ,\
    u32_v*:        u32_vrelease    ,\
    u64_v*:        u64_vrelease    ,\
    default:      (T) \
)(T)

#define v_add(T, X) _Generic((T) ,\
    S8_v*:        S8_vadd     ,\
    i8_v*:        i8_vadd     ,\
    i16_v*:       i16_vadd    ,\
    i32_v*:       i32_vadd    ,\
    i64_v*:       i64_vadd    ,\
    u8_v*:        u8_vadd     ,\
    u16_v*:       u16_vadd    ,\
    u32_v*:       u32_vadd    ,\
    u64_v*:       u64_vadd    ,\
    default:      (T) \
)(T, X)

#endif // CM_GENERIC_CONTAINERS_o
