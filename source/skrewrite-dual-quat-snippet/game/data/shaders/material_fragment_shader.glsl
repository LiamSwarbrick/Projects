#version 450 core

in vec3 normal;
in vec4 colour;
in vec4 uvst;
out vec4 frag_col;

layout (binding = 0) uniform sampler2D diffuse_map;

void
main(void)
{
    vec4 final_colour = colour * texture(diffuse_map, uvst.xy);
    if (final_colour.a < 0.1)
    {
        discard;
    }
    frag_col = final_colour;
}
