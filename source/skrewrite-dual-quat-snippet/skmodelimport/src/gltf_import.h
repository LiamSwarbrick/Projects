#ifndef GLTF_IMPORT_H
#define GLTF_IMPORT_H

#include "common_types.h"
#include "memory_arena.h"
#include "sae_format_description.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include <windows.h>
#include <stdio.h>

// TODO: Create simple blender animated mesh, figure out best export format, import in this program and export data
// in dual quaternion file format.
// Also, maybe change to use more than one skin.
// WATCH:
// https://www.reddit.com/r/gamedev/comments/g9ra89/can_someone_explain_to_me_the_theory_behind/
// https://www.youtube.com/watch?v=B_6xXNTB7dc&ab_channel=SebastianLague

// Try to rig the spirit tracks link in blender
// also check out the obj groups and fully implement static geometry with the materials and vertex groups

// TOMORROW: https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_020_Skins.md

// LINE 3687  https://github.com/raysan5/raylib/blob/master/src/models.c
// test skin https://github.com/javagl/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_019_SimpleSkin.md
// load game\data\enchange_formats\models\simple_mesh.gltf

// Load meshes / skeletons from GLTF https://www.youtube.com/watch?v=cWo-sghCp8Y

#define MODEL_OUT_DIRECTORY "game/data/models/"
#define TEXTURE_OUT_DIRECTORY "game/data/textures"

int
gltf_import_main(int argc, char** argv)
{
    // Expect separate textures using texture file paths
    if (argc <= 2)
    {
        printf("Need two command line args: Pass gltf file and out dir as parameter e.g \"game/data/exchange_files/models/example.gltf\", \"game/data/models/example."RIG_RUNTIME_FILE_EXT"\".\n");

        return -1;
    }

    char* gltf_path = argv[1];
    char* out_path = argv[2];

    cgltf_options options = { 0 };
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);
    if (result != cgltf_result_success)
    {
        printf("Failed to parse gltf file. cgltf_result: %d\n", result);
        return -1;
    }

    result = cgltf_load_buffers(&options, data, gltf_path);
    if (result != cgltf_result_success)
    {
        printf("CGLTF model: %s failed to load buffers", gltf_path);
        return -1;
    }
    
    // Over-allocate for output buffer
    u8* out_buf = VirtualAlloc(0, Megabytes(2), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    u64* bytes_wrote = 0;

    Memory_Arena temp_buffer = arena_init(VirtualAlloc(0, Megabytes(2), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));

    SAE_Rig_Header sae_header = { 0 };

    // Material data:
    sae_header.material_count = data->materials_count;
    SAE_Material* sae_materials = arena_push_size(&temp_buffer, sae_header.material_count * sizeof(SAE_Material));
    for (u32 m = 0; m < sae_header.material_count; ++m)
    {
        SAE_Material sae_mat = { 1, { "game/data/textures/default_material.png", 0, 0 } };
        Assert(data->materials[i].has_pbr_metallic_roughness);

        // 3 texture maps (only one map implemented currently):
        if (data->materials[i].pbr_metallic_roughness.base_color_texture.texture)
        {
            sae_mat.texture_paths[0] = cstring_cat(TEXTURE_OUT_DIRECTORY, data->materials[i].pbr_metallic_roughness.base_color_texture.name);
        }

        sae_materials[m] = sae_mat;
    }

    // Skinlet data:
    SAE_Skin_Vertex** skin_vertices_arrays = arena_push_size(&temp_buffer, sae_header.skinlet_count * sizeof(SAE_Skin_Vertex*));
    for (int s = 0; s < sae_header.skinlet_count; ++s)
    {
        skin_vertices_arrays[s] = arena_push_size(&temp_buffer, sae_header.skinlet_headers.vertex_count * sizeof(SA7in_Vertex));
    }

    int primitive_index = 0;
    sae_header.skinlet_count = data->meshes_count;
    for (u32 s = 0; s < sae_header.skinlet_count; ++s)
    {
        sae_header.skinlet_headers[s].skinlet_id = s;
        for (u32 p = 0; p < data->meshes[s].primitives_count; ++s)
        {
            for (u32 a = 0; a < data->meshes[i].primitives[p].attributes_count; ++a)
            {
                if (data->meshes[i].primitives[p].attributes[a].type == cgltf_attribute_type_position)
                {
                    cgltf_accessor* acc = data->meshes[i].primitives[p].attributes[a].data;
                    sae_header.skinlet_headers[primitive_index].vertex_count = (u32)acc->count;
                    u32 buffer_size = sae_header.skinlet_headers[primitive_index].vertex_count * 3 * sizeof(f32);
                    
                }
            }
        }
        // sae_header.skinlet_headers[s].vertex_count = data->meshes[s].primitives_count * 3;
        // sae_header.skinlet_headers[s].index_count = data->meshes[s].;
    }



    // sae_header.skinlet_count = data->meshes_count;
    // cgltf_skin gltfskin = data->skins[0];
    // sae_header.joint_count = gltfskin.joints_count;
    
    /*
    // look at cgltf_skin struct
    // for each vertex, store the four most influential weights, then normalize them
    u32 mesh_count = data->meshes_count;
    cgltf_mesh* meshes = data->meshes;
    printf("%s\n", meshes[0].name);
    printf("%I64d\n", meshes[0].primitives_count);
    printf("%s\n", meshes[0].primitives[0].attributes[0].name);
    printf("%s\n", meshes[0].primitives[0].attributes[1].name);
    printf("%s\n", meshes[0].primitives[0].attributes[2].name);
    printf("%f %f %f\n", meshes[0].primitives[0].indices);

    Skin skin0 = { 0 };
    for (u32 i = 0; i < meshes[0].primitives_count; ++i)
    {
        cgltf_accessor* position_data  = meshes[0].primitives->attributes[0].data;
        cgltf_accessor* normal_data    = meshes[0].primitives->attributes[1].data;
        cgltf_accessor* texcoord0_data = meshes[0].primitives->attributes[2].data;
        // Primitive = triangle
        Skin_Vertex v1, v2, v3;
        
    }
    */

    return 0;
}

#endif  // GLTF_IMPORT_H