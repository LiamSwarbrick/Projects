#ifndef SK_COMMON_TYPES_H
#define SK_COMMON_TYPES_H

// IMPORTANT TODO: Use SIMD intrinsics in all the math functions

#include <stdint.h>
#include <math.h>  // fmodf

#define macro inline __attribute__((always_inline))
// #define unused __attribute__((unused))

#if defined(GAME_DEBUG) && defined(GAME_WIN32)
#include "win32/windows_with_undefs.h"
#define STRINGIFY(x) #x           // <--Double macro hack to expand macro in different order
#define TOSTRING(x) STRINGIFY(x)  //    __LINE__ doesn't directly expand to the line number.
#define Assert(expression) if (!(expression)) { MessageBoxA(NULL, "Assertion failed in "__FILE__" at line "TOSTRING(__LINE__)": "TOSTRING(expression), GAME_EXE_NAME": assertion failure", MB_ICONERROR); *(int*)0 = 0; }
#else
#define Assert(expression) if (!(expression)) { *(int*)0 = 0; }
#endif

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8,  b8;
typedef uint16_t u16, b16;
typedef uint32_t u32, b32;
typedef uint64_t u64, b64;

typedef float  f32;
typedef double f64;

#define U8_MAX  0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFF

// NOTE: Convertions to bytes
#define Kilobytes(kilobytes) (kilobytes) * 1024ULL
#define Megabytes(megabytes) (megabytes) * 1048576ULL
#define Gigabytes(gigabytes) (gigabytes) * 1073741824ULL

// NOTE: Get high or low half of ints
#define S32_High(dword) (s16)((dword) >> 16)
#define S32_Low(dword)  (s16)((dword) & 0xFFFF)
#define S64_High(qword) (s32)((qword) >> 32)
#define S64_Low(qword)  (s32)((qword) & 0xFFFFFFFF)
#define U32_High(dword) (u16)((dword) >> 16)
#define U32_Low(dword)  (u16)((dword) & 0xFFFF)
#define U64_High(qword) (u32)((qword) >> 32)
#define U64_Low(qword)  (u32)((qword) & 0xFFFFFFFF)

static macro u32
safe_truncate_u64(u64 value)
{
    Assert(value <= U32_MAX);
    return (u32)value;
}

#define Min(a, b) (a) < (b) ? (a) : (b)
#define Max(a, b) (a) > (b) ? (a) : (b)
#define PI  3.14159265358979323846f
#define TAU 6.28318530717958647692f

typedef struct Recti    { s32 x, y, w, h; } Recti;
typedef struct Vector2i { s32 x, y; }       Vector2i;
typedef struct Vector3i { s32 x, y, z; }    Vector3i;
typedef struct Vector4i { s32 x, y, z, w; } Vector4i;

typedef struct Rect    { f32 x, y, w, h; }  Rect;
typedef struct Vector2 { f32 x, y; }        Vector2;
typedef struct Vector3 { f32 x, y, z; }     Vector3;
typedef struct Vector4 { f32 x, y, z, w; }  Vector4;

typedef Vector4 Versor;  // unit quaternion: w + xi + yj + zk where x^2 + y^2 + z^2 = 1 (rotation by navigation around unit hypersphere because 3D rotation with a 3D number system doesn't have enough degrees of freedom).
// NOTE: Column major (the vectors are columns)
typedef struct { Versor r, d; } Dual_Quaternion;  // r + dƐ
#define DQ_INIT { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } }

typedef struct { f32 m[2][2]; } Matrix2;
typedef struct { f32 m[3][3]; } Matrix3;
typedef struct { f32 m[4][4]; } Matrix4;

#define M4_IDENTITY_INIT { { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } } }
#define M4_IDENTITY (Matrix4)M4_IDENTITY_INIT

static macro f32 to_radians(f32 degrees) { return degrees * (PI / 180.0f); }
static macro f32 to_degrees(f32 radians) { return radians * (180.0f / PI); }
static macro f32 rotation_clamp_radians(f32 radians) { return fmodf(radians, TAU); }
static macro f32 rotation_clamp_degrees(f32 degrees) { return fmodf(degrees, 360.0f); }
static macro f32 lerp(f32 from, f32 to, f32 t) { return from + ((to - from) * t); }

//////////////
// Vector i //
//////////////

static macro Vector2i v2i_add(Vector2i u, Vector2i v) { return (Vector2i){ u.x + v.x, u.y + v.y }; }
static macro Vector2i v2i_sub(Vector2i u, Vector2i v) { return (Vector2i){ u.x - v.x, u.y - v.y }; }

static macro Vector3i v3i_add(Vector3i u, Vector3i v) { return (Vector3i){ u.x + v.x, u.y + v.y, u.z + v.z }; }
static macro Vector3i v3i_sub(Vector3i u, Vector3i v) { return (Vector3i){ u.x - v.x, u.y - v.y, u.z - v.z }; }

static macro Vector4i v4i_add(Vector4i u, Vector4i v) { return (Vector4i){ u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w }; }
static macro Vector4i v4i_sub(Vector4i u, Vector4i v) { return (Vector4i){ u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w }; }

//////////////
// Vector 2 //
//////////////

static macro Vector2 v2_add(Vector2 u, Vector2 v) { return (Vector2){ u.x + v.x, u.y + v.y }; }
static macro Vector2 v2_sub(Vector2 u, Vector2 v) { return (Vector2){ u.x - v.x, u.y - v.y }; }
static macro Vector2 v2_mul(Vector2 u, Vector2 v) { return (Vector2){ u.x * v.x, u.y * v.y }; }
static macro Vector2 v2_scale(Vector2 v, f32 s) { return (Vector2){ v.x * s, v.y * s }; }
static macro Vector2 v2_div(Vector2 u, Vector2 v) { return (Vector2){ u.x / v.x, u.y / v.y }; }
static macro Vector2 v2_divs(Vector2 v, f32 s) { return (Vector2){ v.x / s, v.y / s }; }
static macro f32 v2_inner(Vector2 u, Vector2 v) { return (u.x * v.x) + (u.y * v.y); }
static macro Matrix2
v2_outer(Vector2 u, Vector2 v)
{
    Matrix2 result;
    result.m[0][0] = u.x * v.x;
    result.m[0][1] = u.x * v.y;
    result.m[1][0] = u.y * v.x;
    result.m[1][1] = u.y * v.y;
    return result;
}
static macro f32 v2_lengthsq(Vector2 v) { return (v.x * v.x) + (v.y * v.y); }
static macro f32 v2_length(Vector2 v) { return sqrt((v.x * v.x) + (v.y * v.y)); }
static macro Vector2
v2_normalized(Vector2 v)
{
    f32 length = v2_length(v);
    if ((length != 1.0f))
    {
        if (length == 0.0f)
        {
            length = 1.0f;
        }
        return v2_divs(v, length);
    }
    
    return v;
}
static macro b32
v2_compare(Vector2 u, Vector2 v)
{
    if (u.x == v.x && u.y == v.y) { return 1; }
    else { return 0; }
}

//////////////
// Vector 3 //
//////////////

static macro Vector3 v3_add(Vector3 u, Vector3 v) { return (Vector3){ u.x + v.x, u.y + v.y, u.z + v.z }; }
static macro Vector3 v3_sub(Vector3 u, Vector3 v) { return (Vector3){ u.x - v.x, u.y - v.y, u.z - v.z }; }
static macro Vector3 v3_mul(Vector3 u, Vector3 v) { return (Vector3){ u.x * v.x, u.y * v.y, u.z * v.z }; }
static macro Vector3 v3_scale(Vector3 v, f32 s) { return (Vector3){ v.x * s, v.y * s, v.z * s }; }
static macro Vector3 v3_div(Vector3 u, Vector3 v) { return (Vector3){ u.x / v.x, u.y / v.y, u.y / v.y }; }
static macro Vector3 v3_divs(Vector3 v, f32 s) { return (Vector3){ v.x / s, v.y / s, v.z / s }; }
static macro Vector3 v3_negate(Vector3 v) { return (Vector3){ -v.x, -v.y, -v.z }; }
static macro f32 v3_inner(Vector3 u, Vector3 v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.y); }
static macro Matrix3
v3_outer(Vector3 u, Vector3 v)
{
    Matrix3 result;
    result.m[0][0] = u.x * v.x;
    result.m[0][1] = u.x * v.y;
    result.m[0][2] = u.x * v.z;

    result.m[1][0] = u.y * v.x;
    result.m[1][1] = u.y * v.y;
    result.m[1][2] = u.y * v.z;

    result.m[2][0] = u.z * v.x;
    result.m[2][1] = u.z * v.y;
    result.m[2][2] = u.z * v.z;

    return result;
}
static macro Vector3
v3_cross(Vector3 a, Vector3 b)
{
    // https://en.wikipedia.org/wiki/Cross_product#Computing_the_cross_product
    return (Vector3){ (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) };
}
static macro f32 v3_lengthsq(Vector3 v) { return (v.x * v.x) + (v.y * v.y) + (v.z * v.z); }
static macro f32 v3_length(Vector3 v) { return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)); }
static macro Vector3
v3_normalized(Vector3 v)
{
    f32 length = v3_length(v);
    if ((length != 1.0f))
    {
        if (length == 0.0f)
        {
            length = 1.0f;
        }
        return v3_divs(v, length);
    }
    
    return v;
}
static macro b32
v3_compare(Vector3 u, Vector3 v)
{
    if (u.x == v.x && u.y == v.y && u.z == v.z) { return 1; }
    else { return 0; }
}

///////////////////
// NOTE: Matrix2 //
///////////////////

static macro Matrix2
m2_copy(Matrix2 source)
{
    Matrix2 result;
    result.m[0][0] = source.m[0][0];
    result.m[0][1] = source.m[0][1];

    result.m[1][0] = source.m[1][0];
    result.m[1][1] = source.m[1][1];

    return result;
}

///////////////////
// NOTE: Matrix3 //
///////////////////

static macro Matrix3
m3_copy(Matrix3 source)
{
    Matrix3 result;
    result.m[0][0] = source.m[0][0];
    result.m[0][1] = source.m[0][1];
    result.m[0][2] = source.m[0][2];

    result.m[1][0] = source.m[1][0];
    result.m[1][1] = source.m[1][1];
    result.m[1][2] = source.m[1][2];

    result.m[2][0] = source.m[2][0];
    result.m[2][1] = source.m[2][1];
    result.m[2][2] = source.m[2][2];

    return result;
}

///////////////////
// NOTE: Matrix4 //
///////////////////

static macro Matrix4
m4_copy(Matrix4 source)
{
    Matrix4 result;
    result.m[0][0] = source.m[0][0];
    result.m[0][1] = source.m[0][1];
    result.m[0][2] = source.m[0][2];
    result.m[0][3] = source.m[0][3];

    result.m[1][0] = source.m[1][0];
    result.m[1][1] = source.m[1][1];
    result.m[1][2] = source.m[1][2];
    result.m[1][3] = source.m[1][3];

    result.m[2][0] = source.m[2][0];
    result.m[2][1] = source.m[2][1];
    result.m[2][2] = source.m[2][2];
    result.m[2][3] = source.m[2][3];

    result.m[3][0] = source.m[3][0];
    result.m[3][1] = source.m[3][1];
    result.m[3][2] = source.m[3][2];
    result.m[3][3] = source.m[3][3];

    return result;
}

static macro Matrix4
m4_scales(Matrix4 m, f32 s)
{
    Matrix4 result;
    result.m[0][0] = m.m[0][0] *= s;
    result.m[1][1] = m.m[1][1] *= s;
    result.m[2][2] = m.m[2][2] *= s;
    result.m[3][3] = m.m[3][3] *= s;

    return result;
}

static macro Matrix4
m4_scale(Matrix4 m, Vector3 scale)
{
    Matrix4 result;
    result.m[0][0] = m.m[0][0] *= scale.x;
    result.m[1][1] = m.m[1][1] *= scale.y;
    result.m[2][2] = m.m[2][2] *= scale.z;

    return result;
}

static macro Vector4
m4_mulv(Matrix4 m, Vector4 v)
{
    Vector4 result;
    result.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w;
    result.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w;
    result.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w;
    result.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w;
    return result;
}

static macro Matrix4
m4_mul(Matrix4 m1, Matrix4 m2)  // m2*m1
{
    Matrix4 result;

    // Pulled from cglm
    f32 a00 = m1.m[0][0], a01 = m1.m[0][1], a02 = m1.m[0][2], a03 = m1.m[0][3],
          a10 = m1.m[1][0], a11 = m1.m[1][1], a12 = m1.m[1][2], a13 = m1.m[1][3],
          a20 = m1.m[2][0], a21 = m1.m[2][1], a22 = m1.m[2][2], a23 = m1.m[2][3],
          a30 = m1.m[3][0], a31 = m1.m[3][1], a32 = m1.m[3][2], a33 = m1.m[3][3],

          b00 = m2.m[0][0], b01 = m2.m[0][1], b02 = m2.m[0][2], b03 = m2.m[0][3],
          b10 = m2.m[1][0], b11 = m2.m[1][1], b12 = m2.m[1][2], b13 = m2.m[1][3],
          b20 = m2.m[2][0], b21 = m2.m[2][1], b22 = m2.m[2][2], b23 = m2.m[2][3],
          b30 = m2.m[3][0], b31 = m2.m[3][1], b32 = m2.m[3][2], b33 = m2.m[3][3];
    
    result.m[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    result.m[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    result.m[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    result.m[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    result.m[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    result.m[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    result.m[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    result.m[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    result.m[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    result.m[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    result.m[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    result.m[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    result.m[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    result.m[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    result.m[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    result.m[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
    
    return result;
}

static macro Matrix4
m4_inv(Matrix4 mat)
{
    Matrix4 result;
    f32 t[6];
    f32 det;
    f32 a = mat.m[0][0], b = mat.m[0][1], c = mat.m[0][2], d = mat.m[0][3],
          e = mat.m[1][0], f = mat.m[1][1], g = mat.m[1][2], h = mat.m[1][3],
          i = mat.m[2][0], j = mat.m[2][1], k = mat.m[2][2], l = mat.m[2][3],
          m = mat.m[3][0], n = mat.m[3][1], o = mat.m[3][2], p = mat.m[3][3];

    t[0] = k * p - o * l; t[1] = j * p - n * l; t[2] = j * o - n * k;
    t[3] = i * p - m * l; t[4] = i * o - m * k; t[5] = i * n - m * j;

    result.m[0][0] =  f * t[0] - g * t[1] + h * t[2];
    result.m[1][0] =-(e * t[0] - g * t[3] + h * t[4]);
    result.m[2][0] =  e * t[1] - f * t[3] + h * t[5];
    result.m[3][0] =-(e * t[2] - f * t[4] + g * t[5]);

    result.m[0][1] =-(b * t[0] - c * t[1] + d * t[2]);
    result.m[1][1] =  a * t[0] - c * t[3] + d * t[4];
    result.m[2][1] =-(a * t[1] - b * t[3] + d * t[5]);
    result.m[3][1] =  a * t[2] - b * t[4] + c * t[5];

    t[0] = g * p - o * h; t[1] = f * p - n * h; t[2] = f * o - n * g;
    t[3] = e * p - m * h; t[4] = e * o - m * g; t[5] = e * n - m * f;

    result.m[0][2] =  b * t[0] - c * t[1] + d * t[2];
    result.m[1][2] =-(a * t[0] - c * t[3] + d * t[4]);
    result.m[2][2] =  a * t[1] - b * t[3] + d * t[5];
    result.m[3][2] =-(a * t[2] - b * t[4] + c * t[5]);

    t[0] = g * l - k * h; t[1] = f * l - j * h; t[2] = f * k - j * g;
    t[3] = e * l - i * h; t[4] = e * k - i * g; t[5] = e * j - i * f;

    result.m[0][3] =-(b * t[0] - c * t[1] + d * t[2]);
    result.m[1][3] =  a * t[0] - c * t[3] + d * t[4];
    result.m[2][3] =-(a * t[1] - b * t[3] + d * t[5]);
    result.m[3][3] =  a * t[2] - b * t[4] + c * t[5];

    det = 1.0f / (a * result.m[0][0] + b * result.m[1][0]
                + c * result.m[2][0] + d * result.m[3][0]);

    result = m4_scales(result, det);

    return result;
}

static macro Matrix4
m4_frustum(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    Matrix4 result;

    f32 rl = right - left;
    f32 tb = top - bottom;
    f32 fn = far - near;

    result.m[0][0] = (near * 2.0f) / rl;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;
    
    result.m[1][0] = 0.0f;
    result.m[1][1] = (near * 2.0f) / tb;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = (right + left) / rl;
    result.m[2][1] = (top + bottom) / tb;
    result.m[2][2] = -(far + near) / fn;
    result.m[2][3] = -1.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = -(far * near * 2.0f) / fn;
    result.m[3][3] = 0.0f;

    return result;
}

static macro Matrix4
m4_perspective(f32 near, f32 far, f32 fovy, f32 aspect)
{
    f32 top = near * tanf(fovy * 0.5f);
    f32 right = top * aspect;
    Matrix4 result = m4_frustum(-right, right, -top, top, near, far);

    return result;
}

static macro Matrix4
m4_orthographic_fov(f32 near, f32 far, f32 fovy, f32 aspect)
{
    f32 top = fovy / 2.0f;
    f32 right = top * aspect;
    Matrix4 result = m4_frustum(-right, right, -top, top, near, far);
    return result;
}

static macro Matrix4
m4_orthographic(f32 near, f32 far, f32 width, f32 aspect)
{
    f32 fovy = atanf(width / near);
    f32 top = fovy / 2.0f;
    f32 right = top * aspect;
    Matrix4 result = m4_frustum(-right, right, -top, top, near, far);
    return result;
}

static macro Matrix4
m4_rotate(Vector3 axis, f32 angle)
{
    Matrix4 result;
    
    f32 x = axis.x, y = axis.y, z = axis.z;
    f32 length = sqrtf((x * x) + (y * y) + (z * z));
    if ((length != 1.0f) && (length != 0.0f))
    {
        length = 1.0f / length;
        x *= length;
        y *= length;
        z *= length;
    }

    f32 sinres = sinf(angle);
    f32 cosres = cosf(angle);
    f32 t = 1.0f - cosres;

    result.m[0][0] = x*x*t + cosres;
    result.m[0][1] = y*x*t + z*sinres;
    result.m[0][2] = z*x*t - y*sinres;
    result.m[0][3] = 0.0f;

    result.m[1][0] = x*y*t - z*sinres;
    result.m[1][1] = y*y*t + cosres;
    result.m[1][2] = z*y*t + x*sinres;
    result.m[1][3] = 0.0f;

    result.m[2][0] = x*z*t + y*sinres;
    result.m[2][1] = y*z*t - x*sinres;
    result.m[2][2] = z*z*t + cosres;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

static macro Matrix4
m4_rotate_xyz(Vector3 angles)
{
    Matrix4 result = M4_IDENTITY_INIT;

    f32 cosz = cosf(-angles.z);
    f32 sinz = sinf(-angles.z);
    f32 cosy = cosf(-angles.y);
    f32 siny = sinf(-angles.y);
    f32 cosx = cosf(-angles.x);
    f32 sinx = sinf(-angles.x);

    result.m[0][0] = cosz * cosy;
    result.m[1][0] = (cosz * siny * sinx) - (sinz * cosx);
    result.m[2][0] = (cosz * siny * cosx) + (sinz * sinx);

    result.m[0][1] = sinz * cosy;
    result.m[1][1] = (sinz * siny * sinx) + (cosz * cosx);
    result.m[2][1] = (sinz * siny * cosx) - (cosz * sinx);

    result.m[0][2] = -siny;
    result.m[1][2] = cosy * sinx;
    result.m[2][2] = cosy * cosx;

    return result;
}

static macro Matrix4
m4_translate(Matrix4 m, Vector3 t)
{
    Matrix4 result = { { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 1.0f, 0.0f }, { t.x, t.y, t.z, 1.0f } } };
    return result;
}

static macro Matrix4
m4_view(Vector3 position, Vector3 forward, Vector3 up)
{
    /* NOTE: View matrix (4x4) looks like
        [v3 right, v3 up, v3 forward, v3 position]
        [    0   ,    0 ,      0    ,       1    ]
    
    Given a forward and up vector, we can use the cross product to determine the right vector
     */
    Vector3 right = v3_cross(forward, up);
    Matrix4 result = { { { right.x, right.y, right.z, 0.0f }, { up.x, up.y, up.z, 0.0f }, { forward.x, forward.y, forward.z, 0.0f }, { position.x, position.y, position.z, 1.0f } } };
    return result;
}

//////////////////////
// NOTE: Quaternion //
//////////////////////
static macro Versor q_scale(Versor q, f32 s) { return (Versor){ q.x * s, q.y * s, q.z * s, q.w * s }; }
static macro Versor q_add(Versor q1, Versor q2) { return (Versor){ q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w }; }
static macro Versor q_sub(Versor q1, Versor q2) { return (Versor){ q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w }; }
static macro f32 q_dot(Versor q1, Versor q2) { return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w; }
static macro f32 q_normsq(Versor q) { return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w; }
static macro f32 q_norm(Versor q) { return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w); }
static macro Versor
q_normalize(Versor q)
{
    //TODO: Fast normalize using newton iteration on invsqrt
    f32 inv_l = 1.0f / q_norm(q);
    return (Versor){ q.x * inv_l, q.y * inv_l, q.z * inv_l, q.w * inv_l };
}
static macro Versor q_conj(Versor q) { return (Versor){ -q.x, -q.y, -q.z, q.w }; }
static macro Versor
q_inv(Versor q)  // Quaternion conjugate ()
{
    return q_conj(q);
}

static macro Versor
q_mul(Versor a, Versor b)  // a * b
{
    Versor result = {
        a.w * b.x + a.x * b.w + b.y * b.z,
        a.w * b.z - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    };
    return result;
}

static macro Versor
q_rotation_euler(Vector3 axis, float angle_rad)  // q=cos(angle/2) + sin(angle/2)(xi + yj + zk)
{
    float sin_a = sinf(angle_rad/2.0f);
    Versor q = { sin_a * axis.x, sin_a * axis.y, sin_a * axis.z, cosf(angle_rad/2.0f) };
    return q;
}

static macro Vector3
q_rotate_v3(Versor q, Vector3 p)  // q*p*q^-1
{
    Versor result = q_mul(q_mul(q, (Versor){ p.x, p.y, p.z, 0 }), q_inv(q));
    return (Vector3){ result.x, result.y, result.z };
}

static macro Matrix3
q_to_rotation_m3(Versor q)
{
    Vector3 v0 = q_rotate_v3(q, (Vector3){ 1.0f, 0.0f, 0.0f });
    Vector3 v1 = q_rotate_v3(q, (Vector3){ 0.0f, 1.0f, 0.0f });
    Vector3 v2 = q_rotate_v3(q, (Vector3){ 1.0f, 0.0f, 1.0f });
    return (Matrix3){ { { v0.x, v0.y, v0.z }, { v1.x, v1.y, v1.z }, { v2.x, v2.y, v2.z } } };
}

static macro Versor
q_lerp(Versor from, Versor to, f32 t)
{
    return (Versor){ lerp(from.x, from.x, t), lerp(from.y, from.y, t), lerp(from.z, from.z, t), lerp(from.w, from.w, t) };
}

static macro Versor
q_normalized_lerp(Versor from, Versor to, f32 t)  // lerp but normalize so quat stays on the unit hypersphere
{
    return q_normalize(q_lerp(from, to, t));
}

///////////////////////////
// NOTE: Dual_Quaternion //
///////////////////////////

// NOTE: Always ensure the Dual_Quaternions are normalized (aka renormalize after a long chain of operations wher the dqs may start to drift from unit length)
static macro Dual_Quaternion dq_rt(Versor r, Vector3 t) { return (Dual_Quaternion){ r, q_scale(q_mul((Versor){ t.x, t.y, t.z, 0.0f }, r), 0.5f) }; }
static macro f32 dq_dot(Dual_Quaternion a, Dual_Quaternion b) { return q_dot(a.r, b.r); }
static macro Dual_Quaternion dq_scale(Dual_Quaternion q, f32 s) { return (Dual_Quaternion){ q_scale(q.r, s), q_scale(q.d, s) }; }
static macro Dual_Quaternion dq_add(Dual_Quaternion q1, Dual_Quaternion q2) { return (Dual_Quaternion){ q_add(q1.r, q2.r), q_add(q1.d, q2.d) }; }
static macro Dual_Quaternion dq_sub(Dual_Quaternion q1, Dual_Quaternion q2) { return (Dual_Quaternion){ q_sub(q1.r, q2.r), q_sub(q1.d, q2.d) }; }
static macro Dual_Quaternion dq_conj(Dual_Quaternion q) { return (Dual_Quaternion){ q_conj(q.r), q_conj(q.d) }; }
static macro Dual_Quaternion dq_normalize(Dual_Quaternion q)
{
    f32 inv_mag = 1.0f / q_dot(q.r, q.r);
    Assert(inv_mag > (1.0f / 0.000001f));
    return (Dual_Quaternion){ q_scale(q.r, inv_mag), q_sub(q_scale(q.d, inv_mag), q_scale(q.r, q_dot(q.r, q.d) * inv_mag * inv_mag * inv_mag)) };
}
static macro Vector3 dq_get_translation(Dual_Quaternion q)
{
    Versor t = q_mul(q_scale(q.d, 2.0f), q_conj(q.r));
    return (Vector3){ t.x, t.y, t.z };
}
static macro Dual_Quaternion dq_mul(Dual_Quaternion q1, Dual_Quaternion q2)
{
    // Q1Q2 = Q1rQ2r + (Q1rQ2d + Q1dQ2r)Ɛ
    Dual_Quaternion q1q2 = { q_mul(q1.r, q2.r), q_add(q_mul(q1.r, q2.d), q_mul(q1.d, q2.r)) };
    return q1q2;
}
static macro Matrix4 dq_to_m4(Dual_Quaternion q)
{
    Matrix4 m = M4_IDENTITY_INIT;
    f32 w = q.r.w;
    f32 x = q.r.x;
    f32 y = q.r.y;
    f32 z = q.r.z;
    // NOTE: Extract rotational information
    m.m[1][1] = w*w + x*x - y*y - z*z;
    m.m[1][2] = 2*x*y + 2*w*z;
    m.m[1][3] = 2*x*z - 2*w*y;
    m.m[2][1] = 2*x*y - 2*w*z;
    m.m[2][2] = w*w + y*y - x*x - z*z;
    m.m[2][3] = 2*y*z + 2*w*x;
    m.m[3][1] = 2*x*z + 2*w*y;
    m.m[3][2] = 2*y*z - 2*w*x;
    m.m[3][3] = w*w + z*z - x*x - y*y;
    // NOTE: Extract translation information
    Versor t = q_mul(q_scale(q.d, 2.0f), q_conj(q.r));
    m.m[4][1] = t.x;
    m.m[4][2] = t.y;
    m.m[4][3] = t.z;
    return m;
}
/*
static macro Dual_Quaternion dq_dualconj(Dual_Quaternion q) { return (Dual_Quaternion){ q.r, (Versor){ -q.d.x, -q.d.y, -q.d.z, -q.d.w } }; }
static macro Dual_Quaternion dq_bothconj(Dual_Quaternion q) { return (Dual_Quaternion){ q_conj(q.r), (Versor){ q.d.x, q.d.y, q.d.z, -q.d.w } }; }
 */

#endif  // SK_COMMON_TYPES_H
