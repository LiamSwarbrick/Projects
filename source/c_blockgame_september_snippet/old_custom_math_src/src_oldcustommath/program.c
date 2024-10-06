#include "program.h"

#include <GL/gl3w.h>
#include <GL/wglext.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdio.h>
#include <stdlib.h>


void APIENTRY
opengl_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        printf("[OpenGL callback]%s\n", message);
        printf("OpenGL error severity was high and needs fixing!\n");
        abort();
    }
}

// Vertex Data
f32 vertices[] =
{
    -1.0, -1.0, 0.0f, 0.0f, 0.0f,
     1.0,  1.0, 0.0f, 1.0f, 1.0f,
    -1.0,  1.0, 0.0f, 0.0f, 1.0f,
     1.0, -1.0, 0.0f, 1.0f, 0.0f
};
u32 indices[] =
{
    0, 1, 2, 0, 3, 1
};

// FPS Camera:
static vec3 cam_pos = { 0.0f, 0.0f, 0.0f };
static f32 pitch = 0.0f;
static f32 yaw = 0.0f;

void
init()
{
    // OpenGL renderer setup
    {
        printf("OpenGL\n{\n");
        printf("  Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("  Renderer: %s\n", glGetString(GL_RENDERER));
        printf("  Version:  %s\n}\n", glGetString(GL_VERSION));
        printf("\nWindow Dimensions(%d, %d)\nAspect:%f\n", program.w, program.h, program.aspect_ratio);

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, 1);

        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);  // Counter-clockwise is the default, but I've forgetten this before so I prefer being explicit
    }

    // Define Quad mesh
    {
        // Vertex buffers
        u32 vao, vbo, ebo;
        {
            glCreateVertexArrays(1, &vao);
            glCreateBuffers(1, &vbo);
            glCreateBuffers(1, &ebo);

            glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_MAP_WRITE_BIT);  // Fixed sized buffer that can be written to if need be.
            glNamedBufferStorage(ebo, sizeof(indices), indices, GL_MAP_WRITE_BIT);

            // position
            glEnableVertexArrayAttrib(vao, 0);
            glVertexArrayAttribBinding(vao, 0, 0);
            glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

            // uv
            glEnableVertexArrayAttrib(vao, 1);
            glVertexArrayAttribBinding(vao, 1, 0);
            glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(f32));

            glVertexArrayVertexBuffer(vao, 0, vbo, 0, 5 * sizeof(f32));
            glVertexArrayElementBuffer(vao, ebo);
        }

        // Texture data
        u32 tex;
        {    
            const char* filename = "data/image1.png";

            stbi_set_flip_vertically_on_load(1);  // 3D coords use +y=up, so image needs to be flipped

            // Assume 8-bit depth per channel
            int w, h, c;
            u8* data = stbi_load(filename, &w, &h, &c, 4);

            if (data == NULL)
            {
                printf("[Error]Trying to load \'%s\' failed.", filename);
                ExitProcess(1);
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &tex);

            glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTextureStorage2D(tex, 1, GL_RGBA8, w, h);
            glTextureSubImage2D(tex, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateTextureMipmap(tex);

            stbi_image_free(data);
        }

        program.vao = vao;
        program.vbo = vbo;
        program.ebo = ebo;
        program.tex = tex;
    }

    // Define shader program for the mesh
    {
        u32 shader_program;
        const char* opengl_debug_name = "Basic_Shader1";

        u32 vertex_shader;
        u32 fragment_shader;

        char* vertex_src =
        "#version 450 core\n"
        "\n"
        "layout (location = 0) in vec3 v_position;\n"
        "layout (location = 1) in vec2 v_uv;\n"
        "\n"
        "layout (location = 0) uniform mat4 mvp;\n"
        "\n"
        "out vec2 uv;\n"
        "\n"
        "void main(){\n"
        "   gl_Position = mvp * vec4(v_position.x, v_position.y, v_position.z, 1.0);\n"
        "   uv = v_uv;\n"
        "}\0";

        char* fragment_src =
        "#version 450 core\n"
        "\n"
        "in vec2 uv;\n"
        "out vec4 frag_color;\n"
        "\n"
        "layout (binding = 0) uniform sampler2D diffuse_map;\n"
        "\n"
        "void main(){\n"
        "   frag_color = texture(diffuse_map, vec2(uv.x, 1-uv.y));\n"
        "}\0";

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, (const char* const*)&vertex_src, NULL);
        glCompileShader(vertex_shader);
        {
            // Check for failure
            char info_log[1024];
            int vertex_compilation_success;
            glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compilation_success);
            if (!vertex_compilation_success)
            {
                glGetShaderInfoLog(vertex_shader, 1024, NULL, info_log);
                printf(info_log);
            }
        }

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, (const char* const*)&fragment_src, NULL);
        glCompileShader(fragment_shader);
        {
            // Check for failure
            char info_log[1024];
            int fragment_compilation_success;
            glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compilation_success);
            if (fragment_compilation_success)
            {
                glGetShaderInfoLog(fragment_shader, 1024, NULL, info_log);
                printf(info_log);
            }
        }

        shader_program = glCreateProgram();
        glObjectLabel(GL_PROGRAM, shader_program, -1, opengl_debug_name);

        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        {
            // Check for failure
            char info_log[1024];
            int program_success;
            glGetProgramiv(shader_program, GL_LINK_STATUS, &program_success);
            if (!program_success)
            {
                glGetProgramInfoLog(shader_program, 1024, NULL, info_log);
                printf(info_log);
            }
        }

        glDetachShader(shader_program, vertex_shader);
        glDeleteShader(vertex_shader);
        glDetachShader(shader_program, fragment_shader);
        glDeleteShader(fragment_shader);

        program.shader_program = shader_program;
    }

}

void
update_and_render()
{
    // Key Toggles
    {
        if (input.keys['E'] && !last_input.keys['E'])
        {
            input.mouse_fps_mode = !input.mouse_fps_mode;
        }
    }

    
    // FPS cam controls
    if (input.mouse_fps_mode)
    {
        // Look around with mouse:
        const f32 sensitivity = 2.0f;
        {
            f32 mouse_motion_x = sensitivity * ((f32)input.mouse_relative_x / (f32)program.w);
            f32 mouse_motion_y = sensitivity * ((f32)input.mouse_relative_y / (f32)program.h);

            yaw += mouse_motion_x;
            pitch += mouse_motion_y;
            
            // Clamp pitch to avoid going upside-down
            if (pitch < -PI/2.0f)     pitch = -PI/2.0f;
            else if (pitch > PI/2.0f) pitch =  PI/2.0f;
        }

        // Move camera with W,A,S,D
        const f32 speed = 10.0f;  // units per second
        {
            // Buttons:
            int w = (int)input.keys['W'];
            int a = (int)input.keys['A'];
            int s = (int)input.keys['S'];
            int d = (int)input.keys['D'];
            int space = (int)input.keys[VK_SPACE];
            int shift = (int)input.keys[VK_SHIFT];

            f32 pos_increment = speed * program.dt;
            if ((w != s) && (a != d))
            {
                // Normalize speed if going diagonally
                pos_increment *= (f32)(1.0 / sqrt(2.0));
            }

            f32 dx = pos_increment * sinf(yaw);
            f32 dz = pos_increment * cosf(yaw);
            cam_pos.x += dx * (w-s) + dz * (d-a);
            cam_pos.z += dx * (d-a) + dz * (s-w);

            cam_pos.y += speed * program.dt * (f32)(space - shift);
        }
    }

    // Rendering
    {
        mat4 proj = mat4_perspective(to_radians(90.0f), program.aspect_ratio, 0.1f, 100.0f);
        mat4 view = mat4_identity();
        view = mat4_mult(mat4_rotation_x(pitch), view);
        view = mat4_mult(mat4_rotation_y(-yaw), view);
        view = mat4_mult(mat4_translation(vec3_negated(cam_pos)), view);
        mat4 pv = mat4_mult(proj, view);


        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program.shader_program);
        glBindVertexArray(program.vao);
        
        mat4 model = mat4_rotation_y(program.time);
        model = mat4_mult(mat4_translation((vec3){ 0.5f, 0.0f, 10.0f }), model);

        mat4 mvp = mat4_mult(pv, model);  // P*V*M

        glBindTextureUnit(0, program.tex);
        glProgramUniformMatrix4fv(program.shader_program, 0, 1, GL_FALSE, (f32*)mvp.m);  // GL_FALSE since column major order

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(u32), GL_UNSIGNED_INT, 0);

    // DEBUG:
        model = mat4_translation((vec3){ 0.5f, 0.0f, 10.0f });
        mvp = mat4_mult(pv, model);
        glProgramUniformMatrix4fv(program.shader_program, 0, 1, GL_FALSE, (f32*)mvp.m);  // GL_FALSE since column major order
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(u32), GL_UNSIGNED_INT, 0);
    }
}
