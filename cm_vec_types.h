#ifndef CM_VEC_TYPES_H
#define CM_VEC_TYPES_H

// 8 bits signed integer ///////////////////////////////////////////////////////
typedef union Vec2i8 Vec2i8;
union Vec2i8 { struct { i8 x, y; }; i8 v[2]; };

typedef union Vec3i8 Vec3i8;
union Vec3i8 { struct { i8 x, y, z; }; i8 v[3]; };

typedef union Vec4i8 Vec4i8;
union Vec4i8 { struct { i8 w, x, y, z; }; i8 v[4]; };

typedef union Reci8 Reci8;
union Reci8 { struct {i8 x, y, w, h;}; i8 v[4]; };

// 16 bits signed integer //////////////////////////////////////////////////////
typedef union Vec2i16 Vec2i16;
union Vec2i16 { struct { i16 x, y; }; i16 v[2]; };

typedef union Vec3i16 Vec3i16;
union Vec3i16 { struct { i16 x, y, z; }; i16 v[3]; };

typedef union Vec4i16 Vec4i16;
union Vec4i16 { struct { i16 w, x, y, z; }; i16 v[4]; };

typedef union Reci16 Reci16;
union Reci16 { struct {i16 x, y, w, h;}; i16 v[4]; };

// 32 bits signed integer //////////////////////////////////////////////////////
typedef union Vec2i32 Vec2i32;
union Vec2i32 { struct { i32 x, y; }; i32 v[2]; };

typedef union Vec3i32 Vec3i32;
union Vec3i32 { struct { i32 x, y, z; }; i32 v[3]; };

typedef union Vec4i32 Vec4i32;
union Vec4i32 { struct { i32 w, x, y, z; }; i32 v[4]; };

typedef union Reci32 Reci32;
union Reci32 { struct {i32 x, y, w, h;}; i32 v[4]; };

// 64 bits signed integer //////////////////////////////////////////////////////
typedef union Vec2i64 Vec2i64;
union Vec2i64 { struct { i64 x, y; }; i64 v[2]; };

typedef union Vec3i64 Vec3i64;
union Vec3i64 { struct { i64 x, y, z; }; i64 v[3]; };

typedef union Vec4i64 Vec4i64;
union Vec4i64 { struct { i64 w, x, y, z; }; i64 v[4]; };

typedef union Reci64 Reci64;
union Reci64 { struct {i64 x, y, w, h;}; i64 v[4]; };

// 32 bits float////////////////////////////////////////////////////////////////
typedef union Vec2f32 Vec2f32;
union Vec2f32 { struct { f32 x, y; }; f32 v[2]; };

typedef union Vec3f32 Vec3f32;
union Vec3f32 { struct { f32 x, y, z; }; f32 v[3]; };

typedef union Vec4f32 Vec4f32;
union Vec4f32 { struct { f32 w, x, y, z; }; f32 v[4]; };

typedef union Recf32 Recf32;
union Recf32 { struct {f32 x, y, w, h;}; f32 v[4]; };

// 64 bits float////////////////////////////////////////////////////////////////
typedef union Vec2f64 Vec2f64;
union Vec2f64 { struct { f64 x, y; }; f64 v[2]; };

typedef union Vec3f64 Vec3f64;
union Vec3f64 { struct { f64 x, y, z; }; f64 v[3]; };

typedef union Vec4f64 Vec4f64;
union Vec4f64 { struct { f64 w, x, y, z; }; f64 v[4]; };

typedef union Recf64 Recf64;
union Recf64 { struct {f64 x, y, w, h;}; f64 v[4]; };

#endif // CM_VEC_TYPES_H
