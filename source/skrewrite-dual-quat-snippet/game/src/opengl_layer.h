#ifndef SK_OPENGL_LAYER_H
#define SK_OPENGL_LAYER_H

#include <GL/gl3w.h>  // OpenGL 4.5 api

#include "common_types.h"
#include "memory_arena.h"
#include "sakuren.h"
#include "opengl_layer_types.h"
#include "skeletal_animation.h"
#include "sae_format_description.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(size) LIB_ALLOCATION_MALLOC(size)
#define STBI_REALLOC(ptr, size) LIB_ALLOCATION_REALLOC(ptr, size)
#define STBI_FREE(ptr)
#include <stb_image.h>  // Possible TODO: Create own image file readers (probably impracticle)

static void APIENTRY
opengl_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* user_param)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        output_debug_string(arena_push_string(&game->frame_arena, "[OpenGL callback]%s\n", message).data);
        output_debug_string("OpenGL error severity was high and needs fixing!\n");
        ExitProcess(0);
    }
}

Texture2D
load_texture(char* filename)
{
    Texture2D texture = { 0 };

    // NOTE: Uses stbi_image.h to load image
    stbi_set_flip_vertically_on_load(1);
    u32* data = (u32*)stbi_load(filename, &texture.width, &texture.height, &texture.color_channel_count, 0);
    if (data != NULL)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);
        glTextureStorage2D(texture.id, 1, GL_RGBA8, texture.width, texture.height);
        glTextureSubImage2D(texture.id, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, platform->texture_filter);
        glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, platform->texture_filter);
        glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        output_debug_string(arena_push_string(&game->frame_arena, "[Error]load_texture(%s) failed:  Failed to load texture", filename).data);
    }
    
    stbi_image_free(data);

    return texture;
}

Shader
compile_shader_program(char* vertex_string, char* fragment_string, const char* opengl_debug_name)
{
    // NOTE: Compile shaders
    u32 vertex_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_id, 1, (const char* const*)&vertex_string, NULL);
    glCompileShader(vertex_id);
    {
        // NOTE: Check for failure
        char info_log[1024];
        int vertex_success;
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &vertex_success);
        if (!vertex_success)
        {
            glGetShaderInfoLog(vertex_id, 1024, NULL, info_log);
            output_debug_string(info_log);
        }
    }

    u32 fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id, 1, (const char* const*)&fragment_string, NULL);
    glCompileShader(fragment_id);
    {
        // NOTE: Check for failure
        char info_log[1024];
        int fragment_success;
        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &fragment_success);
        if (!fragment_success)
        {
            glGetShaderInfoLog(fragment_id, 1024, NULL, info_log);
            output_debug_string(info_log);
        }
    }
    
    // NOTE: Create and link shader program
    u32 program_id = glCreateProgram();
    glObjectLabel(GL_PROGRAM, program_id, -1, opengl_debug_name);  // NOTE: Negative label length implies label contains a null-terminated string.
    
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);
    {
        // NOTE: Check for failure
        char info_log[1024];
        int program_success;
        glGetProgramiv(program_id, GL_LINK_STATUS, &program_success);
        if (!program_success)
        {
            glGetProgramInfoLog(program_id, 1024, NULL, info_log);
            output_debug_string(info_log);
        }
    }

    // NOTE: Detach then delete shaders
    glDetachShader(program_id, vertex_id);
    glDeleteShader(vertex_id);
    glDetachShader(program_id, fragment_id);
    glDeleteShader(fragment_id);

    Shader shader = { program_id };
    return shader;
}

// Rigged mesh

void
load_rig(char* sae_path, Vector3 highlights[PALETTE_STYLES_COUNT], Vector3 shadows[PALETTE_STYLES_COUNT], Memory_Arena* asset_arena, Rig* out_rig)
{
    // NOTE: Single mesh files
    Rig rig = { 0 };
    u8* file_buffer = DEBUG_read_entire_file(sae_path);
    u8* cursor = file_buffer;
    SAE_Rig_Header header = sae_format_read_header(file_buffer);
    cursor += SAE_RIG_HEADER_FILESIZE;

    // Material format
    rig.skin.materials = arena_push_size(asset_arena, header.material_count * sizeof(Material));

    for (u32 m = 0; m < header.material_count; ++m)
    {
        SAE_Material sae_mat;
        sae_mat.map_count = cursor[0];
        cursor += sizeof(u32);
        copy_memory(cursor, MATERIAL_MAX_MAPS * 256, sae_mat.texture_paths);
        cursor += 3 * 256;

        for (int t = 0; t < sae_map.map_count; ++t)
        {
            rig.skin.materials[m].maps[t] = load_texture(sae_mat.texture_paths[t]);
        }
    }

    rig.joint_count = header.joint_count;
    rig.clip_count = header.clip_count;
    rig.skin.skinlet_count = headers.skinlet_count;

    // Order skinlets into groups with the same materials
    u32 material_count = 0;
    u32 material_ids[SKIN_MATERIALS_MAX] = { 0 };
    u32 material_id_counts[SKIN_MATERIALS_MAX] = { 0 };
    for (u32 i = 0; i < header.skinlet_count; ++i)
    {
        u32 new_material_id = headers.skinlet_headers[i].material_id;
        b32 new_material = 1;
        for (int j = 0; j < material_count; ++i)
        {
            if (new_material_id == material_ids[j])
            {
                ++material_id_counts[j];
                new_material = 0;
                break;
            }
        }
        if (new_material)
        {
            material_ids[material_count] = new_material_id;
            material_id_counts[material_count] = 1;
            ++material_count;
        }
    }

    Grouped_Array skinlet_grouping = arena_push_grouped_array(asset_arena, material_count, material_id_counts, sizeof(Skinlet));
    for (u32 i = 0; i < header.skinlet_count; ++i)
    {
        Skinlet skinlet;
        skinlet.skinlet_id = header.skinlet_headers[i].skinlet_id;
        skinlet.vertex_count = header.skinlet_headers[i].vertex_count;
        skinlet.index_count = header.skinlet_headers[i].index_count;
        skinlet.material_id = header.skinlet_headers[i].material_id;

        for (u32 j = 0; j < material_count; j++)
        {

            if (header.skinlet_headers[i].material_id == material_ids[j])
            {
                for (u32 k = 0; k < skinlet_grouping.lengths[j]; ++k)
                {

                    if (skinlet_grouping.arrays[j][k] == 0)
                    {
                        skinley_grouping.arrays[j][k] = skinlet;
                        break;
                    }
                }
                break;
            }

        }
    }

    // Load vertex data
    for (u32 i = 0; i < header.skinlet_count; ++i)
    {
        u32 skinlet_id = *((u32*)cursor);
        cursor += sizeof(u32);

        // Find skinlet
        for (u32 m = 0; m < material_count; ++m)
        {

            Skinlet* skinlet_array = skinlet_grouping.arrays[m];
            for (u32 s = 0; s < skinlet_grouping.lengths[m]; ++s)
            {

                if (skinlet_array[s].skinlet_id == skinlet_id);
                {
                    Skinlet skinlet = skinlet_array[s];

                    // Read Skin_Vertex array
                    copy_memory(cursor, skinlet.vertex_count * SAE_SKIN_VERTEX_FILESIZE, skinlet_grouping.arrays[m][s].vertices);
                    cursor += skinlet.vertex_count * SAE_SKIN_VERTEX_FILESIZE;
                    copy_memory(cursor, skinlet.index_count * sizeof(u32), skinlet_grouping.arrays[m][s].indices);
                    cursor += skinlet.index_count * sizeof(u32);

                    break;
                }
            }
        }
        
    }

    rig.skin.skinlets_by_material_id = skinlet_grouping;

    // Load joint names
    copy_memory(cursor, 32 * header.joint_count, rig.joint_names);
    cursor += 32 * header.joint_count;

    // Load animation data
    for (int clip_index = 0; clip_index < header.clip_count; ++clip_index)
    {
        SAE_Clip sae_clip = { 0 };

        copy_memory(cursor, 64, sae_clip.name);
        cursor += 64;
        sae_clip.clip_length = ((f32*)cursor)[0];
        cursor += sizeof(f32);
        
        copy_memory(cursor, header.joint_count * sizeof(u32), sae_clip.joint_keyframe_counts);
        cursor += header.joint_count * sizeof(u32);

        copy_memory(cursor, header.joint_count * sizeof(f32), sae_clip.joint_timestamps);
        cursor += header.joint_count * sizeof(f32);

        for (int i = 0; i < header.joint_count; ++i)
        {
            copy_memory(cursor, header.joint_count * sae_clip.joint_keyframe_counts[i], sae_clip.joint_keyframes[i]);
        }

        Clip clip = { 0 };
        clip.name = sae_clip.name;
        clip.clip_length = sae_clip.clip_length;
        for (int i = 0; i < rig.joint_count; ++i)
        {
            clip.joint_animations[i] = (Joint_Animation){ sae_clip.joint_keyframe_counts[i], sae_clip.joint_timestamps[i], sae_clip.joint_keyframes[i] };
        }
        rig.clips[clip_index] = clip;
    }

    DEBUG_free_file_memory(file_buffer);
    *out_rig = rig;

    //
    // OPENGL SECTION
    //

    u32 vao;
    glCreateVertexArrays(1, vao);

    for (int m = 0; m < rig.skin.skinlets_by_material_id.array_count; ++m)
    {
        u32 length = rig.skin.skinlets_by_material_id.lengths[m];
        u32* vbos = arena_push_size(&game->asset_arena, sizeof(u32) * (length * 2)); 
        u32* ebos = vbos + length;
        glCreateBuffers(length, vbos);
        glCreateBuffers(length, ebos);

        rig.skin.vbos_per_skinlet_group[m] = vbos;
        rig.skin.ebos_per_skinlet_group[m] = ebos;

        for (int i = 0; i < length; ++i)
        {
            Skinlet skinlet = rig.skin.skinlets_by_material_id[m][i];

            glNamedBufferStorage(vbos[i], sizeof(Skin_Vertex) * skinlet.vertex_count, skinlet.vertices, GL_MAP_WRITE_BIT);
            glNamedBufferStorage(ebo[i], sizeof(u32) * skinlet.index_count, skinlet.indices, GL_MAP_WRITE_BIT);
        }

    }

    // PPP NNN CCCC TTTT JJJJ WWWW -> 88 bytes
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);  // position vec3

    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Skin_Vertex, n));  // normal vec3

    glVertexArrayAttribBinding(vao, 2, 0);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(Skin_Vertex, c));  // colour vec4

    glVertexArrayAttribBinding(vao, 3, 0);
    glEnableVertexArrayAttrib(vao, 3);
    glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, offsetof(Skin_Vertex, uvst));  // uvst vec4

    glVertexArrayAttribBinding(vao, 4, 0);
    glEnableVertexArrayAttrib(vao, 4);
    glVertexArrayAttribFormat(vao, 4, 4, GL_UNSIGNED_INT, GL_FALSE, offsetof(Skin_Vertex, joint_ids));  // joint_ids u32[4] (ivec4)

    glVertexArrayAttribBinding(vao, 5, 0);
    glEnableVertexArrayAttrib(vao, 5);
    glVertexArrayAttribFormat(vao, 5, 4, GL_FLOAT, GL_FALSE, offsetof(Skin_Vertex, weights));  // weights f32[4] (vec4)

    // NOTE: Link buffers to vao
    for (int m = 0; m < rig.skin.skinlets_by_material_id.array_count; ++m)
    {
        size_t* strides = arena_push_size(&game->frame_arena, rig.skin.skinlets_by_material_id.lengths[m] * sizeof(size_t));
        GLintptr* offsets = arena_push_size(&game->frame_arena, rig.skin.skinlets_by_material_id.lengths[m] * sizeof(GLintptr));
        for (int i = 0; i < rig.skin.skinlets_by_material_id.lengths[m]; ++i)
        {
            strides[i] = sizeof(Skin_Vertex);
            offsets[i] = (GLintptr)0;
            glVertexArrayElementBuffer(vao, rig.skin.ebos_per_skinlet_group[i]);
        }
        glVertexArrayVertexBuffers(vao, 0, rig.skin.skinlet_count, rig.skin.vbos_per_skinlet_group[m], offsets, strides);
    }

    out_rig = &rig;
}

/*
void
create_mesh(Vertex* vertices, u32* indices, u32 vertex_count, u32 index_count, u32* out_vao, u32* out_vbo, u32* out_ebo)
{
    u32 vao, vbo, ebo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);
    
    glNamedBufferStorage(vbo, sizeof(Vertex) * vertex_count, vertices, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(ebo, sizeof(u32) * index_count, indices, GL_MAP_WRITE_BIT);

    // Specify the vertex layout:
    // NOTE: Vertex position (Vector3) (location = 0)
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // NOTE: Vertex texture coord (Vector2) (location = 1)
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, t));
    
    // NOTE: Vertex normal (Vector3) (location = 2)
    glVertexArrayAttribBinding(vao, 2, 0);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, n));

    // NOTE: Link all together
    glVertexArrayVertexBuffer(vao, 0, vbo, (GLintptr)0, sizeof(Vertex));
    glVertexArrayElementBuffer(vao, ebo);

    *out_vao = vao;
    *out_vbo = vbo;
    *out_ebo = ebo;
}

void
load_mesh(char* filename, Memory_Arena* asset_arena, Vertex** out_vertices, u32** out_indices, u32* out_vertex_count, u32* out_index_count) // NOTE: The reason asset_arena is a parameter it allows more control over where you want the memory to reside, may be useful in certain cases
{
    void* data = DEBUG_read_entire_file(filename);  // .skm (static mesh)

    void* cursor = data;
    *out_vertex_count = ((u32*)cursor)[0];
    *out_index_count  = ((u32*)cursor)[1];
    cursor += sizeof(u32) * 2;
    
    // NOTE: Allocate memory for vertices and indices data
    *out_vertices = arena_push_size(asset_arena, *out_vertex_count * sizeof(Vertex));
    *out_indices = arena_push_size(asset_arena, *out_index_count * sizeof(u32));

    copy_memory(cursor, *out_vertex_count * sizeof(Vertex), *out_vertices);
    cursor += *out_vertex_count * sizeof(Vertex);
    copy_memory(cursor, *out_index_count * sizeof(u32), *out_indices);
    
    DEBUG_free_file_memory(data);
}
macro void
load_mesh_and_create(char* filename, Memory_Arena* asset_arena, Vertex** out_vertices, u32** out_indices, u32* out_vertex_count, u32* out_index_count, u32* out_vao, u32* out_vbo, u32* out_ebo)
{
    load_mesh(filename, asset_arena, out_vertices, out_indices, out_vertex_count, out_index_count);
    create_mesh(*out_vertices, *out_indices, *out_vertex_count, *out_index_count, out_vao, out_vbo, out_ebo);
}
*/

/*
void
create_rig_skin(Skin_Vertex* vertices, u32* indices, u32 vertex_count, u32 index_count, u32* out_vao, u32* out_vbo, u32* out_ebo)
{
    u32 vao, vbo, ebo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);
    
    glNamedBufferStorage(vbo, sizeof(Skin_Vertex) * vertex_count, vertices, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(ebo, sizeof(u32) * index_count, indices, GL_MAP_WRITE_BIT);

    // Specify the vertex layout:
    // NOTE: Vertex position (Vector3) (location = 0)
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // NOTE: Two Vertex texture coords (Vector4) (location = 1)
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Skin_Vertex, t));
    
    // NOTE: Vertex normal (Vector3) (location = 2)
    glVertexArrayAttribBinding(vao, 2, 0);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Skin_Vertex, n));

    // NOTE: Vertex bone_ids (ivec4) (location = 3)
    glVertexArrayAttribBinding(vao, 3, 0);
    glEnableVertexArrayAttrib(vao, 3);
    glVertexArrayAttribFormat(vao, 3, VERTEX_MAX_JOINT_INFLUENCE, GL_UNSIGNED_SHORT, GL_FALSE, offsetof(Skin_Vertex, joint_ids));

    // NOTE: Vertex weights (Vector4) (location = 4)
    glVertexArrayAttribBinding(vao, 4, 0);
    glEnableVertexArrayAttrib(vao, 4);
    glVertexArrayAttribFormat(vao, 4, VERTEX_MAX_JOINT_INFLUENCE, GL_FLOAT, GL_FALSE, offsetof(Skin_Vertex, weights));

    // NOTE: Link all together
    glVertexArrayVertexBuffer(vao, 0, vbo, (GLintptr)0, sizeof(Skin_Vertex));
    glVertexArrayElementBuffer(vao, ebo);
    
    *out_vao = vao;
    *out_vbo = vbo;
    *out_ebo = ebo;
}
*/
/*
void
create_rig()  // read rig data file TODO: Create rig data format
{

}
*/

/* rewrite
// Skeletal Mesh
void
create_skeleton_mesh(Skeleton_Vertex* vertices, u32* indices, u32 vertex_count, u32 index_count, Bone* bones, u32 bone_count, u32* out_vao, u32* out_vbo, u32* out_ebo)
{
    u32 vao, vbo, ebo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);
    
    glNamedBufferStorage(vbo, sizeof(Skeleton_Vertex) * vertex_count, vertices, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(ebo, sizeof(u32) * index_count, indices, GL_MAP_WRITE_BIT);

    // Specify the vertex layout:
    // NOTE: Vertex position (Vector3) (location = 0)
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // NOTE: Vertex texture coord (Vector2) (location = 1)
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Skeleton_Vertex, t));
    
    // NOTE: Vertex normal (Vector3) (location = 2)
    glVertexArrayAttribBinding(vao, 2, 0);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Skeleton_Vertex, n));
    
    // NOTE: Vertex bone_ids
    glVertexArrayAttribBinding(vao, 3, 0);
    glEnableVertexArrayAttrib(vao, 3);
    glVertexArrayAttribFormat(vao, 3, VERTEX_MAX_BONE_INFLUENCE, GL_UNSIGNED_SHORT, GL_FALSE, offsetof(Skeleton_Vertex, bone_ids));

    // NOTE: Vertex weights
    glVertexArrayAttribBinding(vao, 4, 0);
    glEnableVertexArrayAttrib(vao, 4);
    glVertexArrayAttribFormat(vao, 4, VERTEX_MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, offsetof(Skeleton_Vertex, weights));

    // NOTE: Link all together
    glVertexArrayVertexBuffer(vao, 0, vbo, (GLintptr)0, sizeof(Skeleton_Vertex));
    glVertexArrayElementBuffer(vao, ebo);

    *out_vao = vao;
    *out_vbo = vbo;
    *out_ebo = ebo;
}
*/

#endif  // SK_OPENGL_LAYER_H
