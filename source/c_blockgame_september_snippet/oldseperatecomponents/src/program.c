#include "program.h"
#include "shader_code.h"
#include "block.h"

#include <GL/gl3w.h>
#include <GL/wglext.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <assert.h>


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

// FPS Camera:
static vec3 cam_pos = { 0.0f, 0.0f, 0.0f };
static f32 pitch = 0.0f;
static f32 yaw = 0.0f;

u32 vao, position_vbo, texcoord_vbo, shade_values_vbo, ebo;
u32 texture_array;
u32 shader_program;

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
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);  // Counter-clockwise is the default, but I've forgetten this before so I prefer being explicit
    }

    define_blocks();

    // Define Quad mesh
    {
        Block_Definition block = block_defs[BLOCK_red_flower];
        // Vertex buffers (non-interleaved for fun)
        {
            glCreateVertexArrays(1, &vao);
            glCreateBuffers(1, &position_vbo);
            glCreateBuffers(1, &texcoord_vbo);
            glCreateBuffers(1, &shade_values_vbo);
            glCreateBuffers(1, &ebo);
            
            assert(block.positions != NULL);
            assert(block.tex_coords != NULL);
            assert(block.shade_values != NULL);
            assert(block.indices != NULL);

            glNamedBufferStorage(position_vbo, block.positions_size, block.positions, GL_MAP_WRITE_BIT);
            glNamedBufferStorage(texcoord_vbo, block.tex_coords_size, block.tex_coords, GL_MAP_WRITE_BIT);
            glNamedBufferStorage(shade_values_vbo, block.shade_values_size, block.shade_values, GL_MAP_WRITE_BIT);
            glNamedBufferStorage(ebo, block.indices_size, block.indices, GL_MAP_WRITE_BIT);
            // Position attribute (0)
            glEnableVertexArrayAttrib(vao, 0);
            glVertexArrayAttribBinding(vao, 0, 0);
            glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayVertexBuffer(vao, 0, position_vbo, 0, 3 * sizeof(f32));

            // UV attribute (1)
            glEnableVertexArrayAttrib(vao, 1);
            glVertexArrayAttribBinding(vao, 1, 1);
            glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayVertexBuffer(vao, 1, texcoord_vbo, 0, 3 * sizeof(f32));

            // Shade attribute (2)
            glEnableVertexArrayAttrib(vao, 2);
            glVertexArrayAttribBinding(vao, 2, 2);
            glVertexArrayAttribFormat(vao, 2, 1, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayVertexBuffer(vao, 2, shade_values_vbo, 0, 1 * sizeof(f32));

            // Element buffer
            glVertexArrayElementBuffer(vao, ebo);
        }

        // // Texture data
        // u32 tex;
        // {    
        //     const char* filename = "data/image1.png";

        //     stbi_set_flip_vertically_on_load(1);  // 3D coords use +y=up, so image needs to be flipped

        //     // Assume 8-bit depth per channel
        //     int w, h, c;
        //     u8* data = stbi_load(filename, &w, &h, &c, 4);

        //     if (data == NULL)
        //     {
        //         printf("[Error]Trying to load \'%s\' failed.", filename);
        //         ExitProcess(1);
        //     }

        //     glCreateTextures(GL_TEXTURE_2D, 1, &tex);

        //     glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //     glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //     glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //     glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //     glTextureStorage2D(tex, 1, GL_RGBA8, w, h);
        //     glTextureSubImage2D(tex, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //     glGenerateTextureMipmap(tex);

        //     stbi_image_free(data);
        // }

        // Load Texture array
        {
            const char* filename = "data/texturearray.png";

            // Don't flip (we want texture 0 to be the top of the png file)
            stbi_set_flip_vertically_on_load(0);

            // Assume 8-bit depth per channel
            int w, h, c;
            u8* texels = stbi_load(filename, &w, &h, &c, 4);

            if (texels == NULL)
            {
                printf("[Error]Trying to load \'%s\' failed.", filename);
                ExitProcess(1);
            }

            // w is the dimension of each texture, h/w gives us the amount of textures e.g. 64/16 = 4 textures
            int layer_count = h / w;
            
            // Generate and bind texture array (since bindless seems to crash when calling glTextureParameteri with texture arrays?)
            glGenTextures(1, &texture_array);
            glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);

            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, w, w, layer_count);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, w, w, layer_count, GL_RGBA, GL_UNSIGNED_BYTE, texels);

            // Free the texture data after uploading to GPU
            stbi_image_free(texels);

            // Unbind texture
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

            // // OLD CODE THAT CRASHES:
            // glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture_array);
            
            // glTextureParameteri(texture_array, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTextureParameteri(texture_array, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // glTextureParameteri(texture_array, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTextureParameteri(texture_array, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // glTextureStorage3D(texture_array, 1, GL_RGBA8, w, h, layer_count);
            // glTextureSubImage3D(texture_array, 0, 0, 0, 0, w, h, layer_count, GL_RGBA, GL_UNSIGNED_BYTE, texels); 
        }
    }




    // Define shader program for the mesh
    {
        const char* opengl_debug_name = "Basic_Shader1";

        u32 vertex_shader;
        u32 fragment_shader;

        char* vertex_src = blocks_shader_vertex_src;
        char* fragment_src = blocks_shader_fragment_src;

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
            ShowCursor(!input.mouse_fps_mode);
            SetCursorPos(input.mouse_fps_pivot_x, input.mouse_fps_pivot_y);  // Recentre cursor
        }
    }
    
    // First person camera controls
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
        f32 speed = 10.0f;  // units per second
        {
            // Buttons:
            int w = (int)input.keys['W'];
            int a = (int)input.keys['A'];
            int s = (int)input.keys['S'];
            int d = (int)input.keys['D'];
            int space = (int)input.keys[VK_SPACE];
            int shift = (int)input.keys[VK_SHIFT];

            if (input.keys[VK_CONTROL]) speed *= 4.0f;

            f32 pos_increment = speed * program.dt;
            if ((w != s) && (a != d))
            {
                // Normalize speed if going diagonally
                pos_increment *= (f32)(1.0 / sqrt(2.0));
            }

            f32 dx = pos_increment * sinf(yaw);
            f32 dz = pos_increment * cosf(yaw);
            cam_pos[0] += dx * (w-s) + dz * (d-a);
            cam_pos[2] += dx * (d-a) + dz * (s-w);

            cam_pos[1] += speed * program.dt * (f32)(space - shift);
        }
    }
    
    // Rendering
    {
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        glm_rotate(view, pitch, (vec3){ 1.0f, 0.0f, 0.0f });
        glm_rotate(view, yaw, (vec3){ 0.0f, 1.0f, 0.0f });
        glm_translate(view, (vec3){ -cam_pos[0], -cam_pos[1], -cam_pos[2] });
        
        // mat4 proj = mat4_perspective(to_radians(90.0f), program.aspect_ratio, 0.1f, 100.0f);
        mat4 proj = GLM_MAT4_IDENTITY_INIT;
        glm_perspective(glm_rad(70.0f), program.aspect_ratio, 0.1f, 100.0f, proj);


        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        // glm_rotate(model, program.time, (vec3){ 0.33f, 0.33f, 0.33f });
        glm_translate(model, (vec3){ 0.5f, 0.0f, -10.0f });
        glm_rotate_y(model, program.time, model);

        mat4 mvp;
        glm_mat4_mul(view, model, mvp);
        glm_mat4_mul(proj, mvp, mvp);
        
        glBindTextureUnit(0, texture_array);
        glProgramUniformMatrix4fv(shader_program, 0, 1, GL_FALSE, (f32*)mvp);  // GL_FALSE since column major order

        glDrawElements(GL_TRIANGLES, block_defs[BLOCK_red_flower].indices_size / sizeof(u32), GL_UNSIGNED_INT, 0);
    }
}
