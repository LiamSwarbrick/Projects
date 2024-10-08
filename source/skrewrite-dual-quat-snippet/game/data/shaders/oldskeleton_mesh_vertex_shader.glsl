#version 450 core

const int MAX_JOINT 50;

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec2 vertex_uv;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in ivec4 joint_ids;
layout (location = 4) in vec4 weights;

out vec2 uv;
out vec3 normal;

layout (location = 0) uniform mat4 mvp;
layout (location = 4) uniform mat4 joint_transforms[MAX_joint_COUNT];

void
main(void)
{
    // Weighted transforms (note: weights sum to 1)
    mat4 t0 = weights.x * joint_transforms[joint_ids.x];
    mat4 t1 = weights.y * joint_transforms[joint_ids.y];
    mat4 t2 = weights.z * joint_transforms[joint_ids.z];
    mat4 t3 = weights.w * joint_transforms[joint_ids.w];
    // Weighted average
    vec4 new_position = t0 * vec4(vertex_pos, 1.0)
                      + t1 * vec4(vertex_pos, 1.0)
                      + t2 * vec4(vertex_pos, 1.0)
                      + t3 * vec4(vertex_pos, 1.0);

    vec4 new_normal = t0 * vec4(vertex_normal, 1.0)
                    + t1 * vec4(vertex_normal, 1.0)
                    + t2 * vec4(vertex_normal, 1.0)
                    + t3 * vec4(vertex_normal, 1.0);

    gl_Position = mvp * vec4(new_position.xyz, 1.0);  // incase new_position.w is not exactly 1.0
    uv = vertex_uv;
    normal = vec4(new_normal.xyz, 1.0);
}
