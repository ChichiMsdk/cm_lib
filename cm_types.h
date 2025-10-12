#ifndef CM_TYPES_H
#define CM_TYPES_H

#include <stdint.h>

// Classic types ///////////////////////////////////////////////////////////////
typedef signed char         i8;
typedef short               i16;
typedef int                 i32;
typedef long long           i64;

typedef signed char         s8;
typedef short               s16;
typedef int                 s32;
typedef long long           s64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef float               f32;
typedef double              f64;

#define _imax  intmax_t;
#define _umax  uintmax_t;

#define _u64max 0xffffffffffffffffull
#define _u32max 0xffffffff
#define _u16max 0xffff
#define _u8max  0xff

#define _i64max 0x7fffffffffffffffull
#define _i32max 0x7fffffff
#define _i16max 0x7fff
#define _i8max  0x7f

#define _BITMASK1 0x00000001
#define _BITMASK2 0x00000003
#define _BITMASK3 0x00000007
#define _BITMASK4 0x0000000f
#define _BITMASK5 0x0000001f
#define _BITMASK6 0x0000003f


// Vector types ////////////////////////////////////////////////////////////////
#include "cm_vec_types.h"

// String types ////////////////////////////////////////////////////////////////
#include "cm_string_types.h"

#endif // CM_TYPES_H
