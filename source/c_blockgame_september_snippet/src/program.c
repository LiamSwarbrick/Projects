#include "program.h"
#include "shader_code.h"
#include "block.h"
#include "chunk.h"
#include "world.h"

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
static f32 yaw = PI / 2.0f;

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

    define_blocks_templates();
    init_chunk_worker_buffers();
    program.render_as_wireframe = 0;
    program.chunk_load_distance = 1;
    program.chunk_unload_distance = 4;
    program.fov_y = glm_rad(70.0f);
    
    assert(program.chunk_load_distance < program.chunk_unload_distance);  // stops perpetual loading & unloading softlock

    init_chunk_map(&program.map, 16);

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
        
        // Calculate number of mipmap levels
        int mipmap_levels = 1 + (int)floor(log2(w));

        // Generate and bind texture array (since bindless seems to crash when calling glTextureParameteri with texture arrays?)
        glGenTextures(1, &program.block_texture_array);
        glBindTexture(GL_TEXTURE_2D_ARRAY, program.block_texture_array);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipmap_levels, GL_RGBA8, w, w, layer_count);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, w, w, layer_count, GL_RGBA, GL_UNSIGNED_BYTE, texels);

        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        // Free the texture data after uploading to GPU
        stbi_image_free(texels);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }


    // Define shader program for the mesh
    {
        const char* opengl_debug_name = "Ground_Shader";

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

        program.chunk_shader = glCreateProgram();
        glObjectLabel(GL_PROGRAM, program.chunk_shader, -1, opengl_debug_name);

        glAttachShader(program.chunk_shader, vertex_shader);
        glAttachShader(program.chunk_shader, fragment_shader);
        glLinkProgram(program.chunk_shader);
        {
            // Check for failure
            char info_log[1024];
            int program_success;
            glGetProgramiv(program.chunk_shader, GL_LINK_STATUS, &program_success);
            if (!program_success)
            {
                glGetProgramInfoLog(program.chunk_shader, 1024, NULL, info_log);
                printf(info_log);
            }
        }

        glDetachShader(program.chunk_shader, vertex_shader);
        glDeleteShader(vertex_shader);
        glDetachShader(program.chunk_shader, fragment_shader);
        glDeleteShader(fragment_shader);
    }


}

void
update_and_render()
{
    // Key Toggles  (E for first person mouse, F1 for wireframe render)
    {
        if (input.keys['E'] && !last_input.keys['E'])
        {
            input.mouse_fps_mode = !input.mouse_fps_mode;
            ShowCursor(!input.mouse_fps_mode);
            SetCursorPos(input.mouse_fps_pivot_x, input.mouse_fps_pivot_y);  // Recentre cursor
        }

        if (input.keys[VK_F1] && !last_input.keys[VK_F1])
        {
            program.render_as_wireframe = !program.render_as_wireframe;
            if (program.render_as_wireframe)
            {
                glLineWidth(2.0f);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glLineWidth(1.0f);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }
    
    f32 current_fov_y = program.fov_y;
    if (input.keys['C'])
        current_fov_y = glm_rad(15.0f);

    load_near_chunks(&program.map, cam_pos, program.chunk_load_distance);
    unload_far_chunks(&program.map, cam_pos, program.chunk_unload_distance);


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

            // Normalise yaw to [0, 2PI)
            if (yaw < 0.0f)
                yaw += 2.0f * PI;
            yaw = fmodf(yaw, 2.0f * PI);
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

            if (input.keys[VK_CONTROL]) speed *= 10.0f;

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

    // Set window text to direction player is facing:
    {
        if (yaw >= 7 * PI / 4 || yaw < PI / 4)
        {
            SetWindowText(program.window_handle, "North");
        }
        else if (yaw >= PI / 4 && yaw < 3 * PI / 4)
        {
            SetWindowText(program.window_handle, "East");
        }
        else if (yaw >= 3 * PI / 4 && yaw < 5 *PI / 4)
        {
            SetWindowText(program.window_handle, "South");
        }
        else
        {
            SetWindowText(program.window_handle, "West");
        }
    }

    // Rendering
    {
        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate camera matrix:
        mat4 camera_matrix;
        {
            // View matrix: Move to player position and orientation
            mat4 view = GLM_MAT4_IDENTITY_INIT;
            glm_rotate(view, pitch, (vec3){ 1.0f, 0.0f, 0.0f });
            glm_rotate(view, yaw, (vec3){ 0.0f, 1.0f, 0.0f });
            glm_translate(view, (vec3){ -cam_pos[0], -cam_pos[1], -cam_pos[2] });
            
            // Projection Matrix: Project world space to screen space
            mat4 proj = GLM_MAT4_IDENTITY_INIT;
            glm_perspective(current_fov_y, program.aspect_ratio, 0.1f, 1000.0f, proj);

            glm_mat4_mul(proj, view, camera_matrix);
        }

        
        // Draw chunks (TODO: Only render chunks within view frustum)
        {
            glUseProgram(program.chunk_shader);
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            mat4 mvp;
            glm_mat4_mul(camera_matrix, model, mvp);
            
            for (u64 i = 0; i < program.map.capacity; ++i)
            {
                Chunk* chunk = program.map.entries[i].chunk;
                if (chunk)
                {
                    // If a chunk is dirty, regen mesh
                    if (chunk->dirty)
                    {
                        gen_chunk_mesh(chunk, &program.map, 1);
                    }
                    
                    // Only render if there are indices to draw
                    if (chunk->indices_size > 0)
                    {
                        glBindVertexArray(chunk->vao);
                        glBindTextureUnit(0, program.block_texture_array);
                        glProgramUniformMatrix4fv(program.chunk_shader, 0, 1, GL_FALSE, (f32*)mvp);  // GL_FALSE since column major order

                        glDrawElements(GL_TRIANGLES, chunk->indices_size / sizeof(u32), GL_UNSIGNED_INT, 0);
                    }
                }
            }
        }


    }
}
