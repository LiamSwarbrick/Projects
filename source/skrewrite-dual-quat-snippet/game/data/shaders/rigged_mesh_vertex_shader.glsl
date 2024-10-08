#version 450 core

struct Dual_Quaternion  // alternative would be mat2x4
{
    vec4 r;
    vec4 d;
};

// vec4
// q_mul(vec4 a, vec4 b)  // quaternions
// {
//     vec4 result = vec4(
//         a.w * b.x + a.x * b.w + b.y * b.z,
//         a.w * b.z - a.x * b.z + a.y * b.w + a.z * b.x,
//         a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
//         a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
//     );
//     return result;
// }

// Dual_Quaternion
// dq_mul(Dual_Quaternion a, Dual_Quaternion b)
// {
//     Dual_Quaternion ab = Dual_Quaternion( q_mul(a.r, b.r), q_mul(a.r, b.d) + q_mul(a.d, b.r) );
//     return ab;
// }

Dual_Quaternion
dq_scale(Dual_Quaternion q, float s)
{
    return Dual_Quaternion( s * q.r, s * q.d );
}

Dual_Quaternion
sum_of_4_dqs(Dual_Quaternion a, Dual_Quaternion b, Dual_Quaternion c, Dual_Quaternion d)
{
    return Dual_Quaternion( a.r + b.r + c.r + d.r, a.d + b.d + c.d + d.d );
}

Dual_Quaternion
dq_normalize(Dual_Quaternion q)
{
    f32 inv_mag = 1.0 / dot(q.r, q.r);

    return Dual_Quaternion( inv_mag * q.r, inv_mag * q.d );
}

////////////////////////////////////////////////////////////////////////////////////////
// pnctjw
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec4 vertex_colour;
layout (location = 3) in vec4 vertex_uvst;
layout (location = 4) in ivec4 joint_ids;
layout (location = 5) in vec4 weights;

out vec3 normal;
out vec4 colour;
out vec4 uvst;

layout (location = 0) uniform mat4 mvp;
layout (binding = 0) uniform Dual_Quaternion joint_transforms[64];
/* EXAMPLE:
layout (binding = 1) uniform material_buffer
{
    vec4 material_diffuse;
    vec4 material_emissive;
};*/

// look at shader storage blocks: https://www.geeks3d.com/20140704/tutorial-introduction-to-opengl-4-3-shader-storage-buffers-objects-ssbo-demo/
// glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ...);
void
main(void)
{
    Dual_Quaternion b;
    b = dq_normalize(sum_of_4_dqs(
                      dq_scale(weights.x, joint_transforms[joint_ids.x]),
                      dq_scale(weights.y, joint_transforms[joint_ids.y]),
                      dq_scale(weights.z, joint_transforms[joint_ids.z]),
                      dq_scale(weights.w, joint_transforms[joint_ids.w])
                      ));

    vec3 t = vec3( 2.0 * (-b.d.w * b.r.x + b.d.x * b.r.w - b.d.y * b.r.z + b.d.z * b.r.y),
               2.0 * (-b.d.w * b.r.y + b.d.x * b.r.z + b.d.y * b.r.w - b.d.z * b.r.x),
               2.0 * (-b.d.w * b.r.z + b.d.x * b.r.y + b.d.y * b.r.x + b.d.z * b.r.w) );
    
    mat4 m;
    float xx = 2 * b.r.x * b.r.x;
    float yy = 2 * b.r.y * b.r.y;
    float zz = 2 * b.r.z * b.r.z;
    float xy = 2 * b.r.x * b.r.y;
    float wz = 2 * b.r.w * b.r.z;
    float xz = 2 * b.r.x * b.r.z;
    float wy = 2 * b.r.w * b.r.y;
    float yz = 2 * b.r.y * b.r.z;
    float wx = 2 * b.r.w * b.r.x;
    m[0][0] = 1 - yy - zz;
    m[0][1] = xy + wz;
    m[0][2] = xz - wy;
    m[1][0] = xy - wz;
    m[1][1] = 1 - xx - zz;
    m[1][2] = yz + wx;
    m[2][0] = xz + wy;
    m[2][1] = yz - wx;
    m[2][2] = 1 - xx - yy;
    m[3] = vec4(t, 1.0);

    vec4 new_position = m * vec4(vertex_pos, 1.0);
    vec4 new_normal = m * vec4(vertex_normal, 0.0);

    gl_Position = mvp * new_position;
    normal = new_normal;
    colour = vertex_colour;
    uvst = vertex_uvst;
}
