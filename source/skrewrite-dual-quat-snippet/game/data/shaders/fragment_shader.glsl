#version 450 core

in vec2 uv;
in vec3 normal;
out vec4 frag_col;

layout (binding = 0) uniform sampler2D diffuse_map;

void
main(void)
{
    vec4 final_colour = texture(diffuse_map, uv);
    if (final_colour.a < 0.1)
    {
        discard;
    }
    frag_col = final_colour;
}
