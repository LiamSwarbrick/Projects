#ifndef BLOCKGAME_GRAPHICS_DATA_H
#define BLOCKGAME_GRAPHICS_DATA_H

#include "basic_types.h"


// f32 vertex_positions[] = {
//      0.5f,  0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
// 	-0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
// 	-0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f,
// 	-0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,
// 	-0.5f,  0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
// 	 0.5f,  0.5f, -0.5f,   0.5f, -0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
// };

// f32 tex_coords[] = {
//     0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
// 	0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
// 	0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
// 	0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
// 	0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
// 	0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
// };


// f32 tex_coords[] = {
//     0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
// 	0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
// 	0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
// 	0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
// 	0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
// 	0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
// };

// u32 indices[] = {
// 	 0,  1,  2,  0,  2,  3,  // right
// 	 4,  5,  6,  4,  6,  7,  // left
// 	 10,  9, 8,  11, 10, 8,  // top
// 	12, 13, 14, 12, 14, 15,  // bottom
// 	16, 17, 18, 16, 18, 19,  // front
// 	20, 21, 22, 20, 22, 23,  // back
// };



char* blocks_shader_vertex_src = 
"#version 450 core\n"
"\n"
"layout (location = 0) in vec3 v_position;\n"
"layout (location = 1) in vec3 v_uv;\n"
"layout (location = 2) in float v_shade_value;\n"
"\n"
"layout (location = 0) uniform mat4 mvp;\n"
"\n"
"out vec3 uv;\n"
"out float shade_value;\n"
"\n"
"void main(){\n"
"   gl_Position = mvp * vec4(v_position.x, v_position.y, v_position.z, 1.0);\n"
"   uv = v_uv;\n"
"   shade_value = v_shade_value;\n"
"}\0";

char* blocks_shader_fragment_src =
"#version 450 core\n"
"\n"
"in vec3 uv;\n"
"in float shade_value;\n"
"out vec4 frag_color;\n"
"\n"
"layout (binding = 0) uniform sampler2DArray diffuse_maps;\n"
"\n"
"void main(){\n"
"   frag_color = shade_value * texture(diffuse_maps, vec3(uv.x, 1-uv.y, uv.z));\n"
"}\0";

#endif  // BLOCKGAME_GRAPHICS_DATA_H
