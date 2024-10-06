#ifndef BLOCKGAME_GRAPHICS_DATA_H
#define BLOCKGAME_GRAPHICS_DATA_H

#include "basic_types.h"

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
"   vec4 col = shade_value * texture(diffuse_maps, vec3(uv.x, 1-uv.y, uv.z));\n"
"   if (col.a < 0.1) {\n"
"       discard;\n"
"   }\n"
"   frag_color = col;\n"
"}\0";

#endif  // BLOCKGAME_GRAPHICS_DATA_H
