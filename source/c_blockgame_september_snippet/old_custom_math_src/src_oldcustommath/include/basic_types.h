#ifndef BLOCKGAME_BASIC_TYPES_H
#define BLOCKGAME_BASIC_TYPES_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  b8;
typedef int16_t b16;
typedef int32_t b32;
typedef int64_t b64;

typedef float    f32;
typedef double   f64;

typedef struct vec3 { f32 x, y, z; } vec3;
// typedef struct vec4 { f32 x, y, z, w; } vec4;

typedef union mat4
{
    struct  // Column major storage (but standard notation: mxy = m(row)(col))
    {       // Therefore set transpose to GL_FALSE when uploading to OpenGL
        // column 0
        f32 m00;
        f32 m10;
        f32 m20;
        f32 m30;

        // column 1
        f32 m01;
        f32 m11;
        f32 m21;
        f32 m31;

        // column 2
        f32 m02;
        f32 m12;
        f32 m22;
        f32 m32;

        // column 3
        f32 m03;
        f32 m13;
        f32 m23;
        f32 m33;
    };

    f32 m[16];  // For uploading to OpenGL;
}
mat4;

static void
print_mat4(mat4 m)
{
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.m00, m.m01, m.m02, m.m03);
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.m10, m.m11, m.m12, m.m13);
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.m20, m.m21, m.m22, m.m23);
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.m30, m.m31, m.m32, m.m33);
}

// OLD ROW MAJOR:
// typedef union {
//     struct {
//         f32 m00, m01, m02, m03;
//         f32 m10, m11, m12, m13;
//         f32 m20, m21, m22, m23;
//         f32 m30, m31, m32, m33;
//     };
//     f32 m[16];
// } mat4;


#define PI  3.14159265358979323846f
static f32 to_radians(f32 degrees) { return degrees * (PI / 180.0f); }
static f32 to_degrees(f32 radians) { return radians * (180.0f / PI); }

static vec3
vec3_negated(vec3 v)
{
    return (vec3){ -v.x, -v.y, -v.z };
}

static vec3
vec3_normalised(vec3 v)
{
    f32 length_squared = v.x * v.x + v.y * v.y + v.z * v.z;
    f32 factor = 1.0f / sqrtf(length_squared);
    v.x *= factor;
    v.y *= factor;
    v.z *= factor;
    return v;
}

static mat4
mat4_identity()
{
    // mat4 identity;
    // identity.m00 = 1.0f; identity.m01 = 0.0f; identity.m02 = 0.0f; identity.m03 = 0.0f;
    // identity.m10 = 0.0f; identity.m11 = 1.0f; identity.m12 = 0.0f; identity.m13 = 0.0f;
    // identity.m20 = 0.0f; identity.m21 = 0.0f; identity.m22 = 1.0f; identity.m23 = 0.0f;
    // identity.m30 = 0.0f; identity.m31 = 0.0f; identity.m32 = 0.0f; identity.m33 = 1.0f;
    // return identity;
    return (mat4){
        .m = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        }
    };
}

static mat4
mat4_mult(mat4 a, mat4 b)
{
    mat4 ab;
    ab.m00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30;
    ab.m01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31;
    ab.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32;
    ab.m03 = a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33;

    ab.m10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30;
    ab.m11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31;
    ab.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32;
    ab.m13 = a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33;

    ab.m20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30;
    ab.m21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31;
    ab.m22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32;
    ab.m23 = a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33;

    ab.m30 = a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30;
    ab.m31 = a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31;
    ab.m32 = a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32;
    ab.m33 = a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33;

    return ab;
}

// static mat4
// mat4_frustum(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane)
// {
//     f32 r_l = (f32)(right - left);
//     f32 t_b = (f32)(top - bottom);
//     f32 f_n = (f32)(far_plane - near_plane);
//     f32 l = (f32)left, r = (f32)right;
//     f32 b = (f32)bottom, t = (f32)top;
//     f32 n = (f32)near_plane, f = (f32)far_plane;

//     mat4 frustum = { 0 };
//     frustum.m00 = 2.0f * n / r_l;
//     frustum.m02 = (r + l)/r_l;
//     frustum.m11 = 2.0f * n / t_b;
//     frustum.m12 = (t + b) / t_b;
//     frustum.m22 = -(f + n) / f_n;
//     frustum.m23 = (-2.0f * f * n) / f_n;
//     frustum.m32 = -1.0f;

//     return frustum;
// }

static mat4
mat4_perspective(f32 fov, f32 aspect, f32 near_plane, f32 far_plane)
{
    f32 tan_half_fov = tanf(fov / 2.0f);
    f32 z_range = far_plane - near_plane;

    mat4 frustum = mat4_identity();
    frustum.m00 = 1.0f / (aspect * tan_half_fov);
    frustum.m11 = 1.0f / tan_half_fov;
    frustum.m22 = -(far_plane + near_plane) / z_range;
    frustum.m23 = -2.0f * far_plane * near_plane / z_range;
    frustum.m32 = -1.0f;
    frustum.m33 = 0.0f;

    return frustum;

    // f64 top = near_plane * tan(fov * 0.5);
    // f64 bottom = -top;
    // f64 right = top * aspect;
    // f64 left = -right;

    // return mat4_frustum(left, right, bottom, top, near_plane, far_plane);
}

static mat4
mat4_translation(vec3 v)
{
    mat4 translation_matrix = mat4_identity();
    translation_matrix.m03 = v.x;
    translation_matrix.m13 = v.y;
    translation_matrix.m23 = v.z;

    return translation_matrix;
}

static mat4
mat4_rotation_x(f32 angle)
{
    f32 cos_a = cosf(angle);
    f32 sin_a = sinf(angle);

    mat4 rotation = mat4_identity();
    rotation.m11 = cos_a;
    rotation.m12 = sin_a;
    rotation.m21 = -sin_a;
    rotation.m22 = cos_a;

    return rotation;
}

static mat4
mat4_rotation_y(f32 angle)
{
    f32 cos_a = cosf(angle);
    f32 sin_a = sinf(angle);

    mat4 rotation = mat4_identity();
    rotation.m00 = cos_a;
    rotation.m02 = -sin_a;
    rotation.m20 = sin_a;
    rotation.m22 = cos_a;

    return rotation;
}

static mat4
mat4_rotation_z(f32 angle)
{
    f32 cos_a = cosf(angle);
    f32 sin_a = sinf(angle);

    mat4 rotation = mat4_identity();
    rotation.m00 = cos_a;
    rotation.m01 = sin_a;
    rotation.m10 = -sin_a;
    rotation.m11 = cos_a;

    return rotation;
}

#endif  // BLOCKGAME_BASIC_TYPES_H
