#ifndef CM_STRING_TYPES_H
#define CM_STRING_TYPES_H

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode { u32 inc; u32 codepoint; };

// 8 bits unsigned string //////////////////////////////////////////////////////
typedef struct S8 S8;
struct S8 { char  *str; u64 len; };

// 32 bits unsigned string /////////////////////////////////////////////////////
typedef struct S32 S32;
struct S32 { u32  *str; u64 len; };

// 64 bits unsigned string //////////////////////////////////////////////////////
typedef struct S64 S64;
struct S64 { u64  *str; u64 len; };

#endif // CM_STRING_TYPES_H
