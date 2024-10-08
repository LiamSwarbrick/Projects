#ifndef SAE_FORMAT_DESCRIPTION_H
#define SAE_FORMAT_DESCRIPTION_H

#include "common_types.h"

typedef struct SAE_Skin_Vertex
{
    Vector3 p;
    Vector3 n;
    Vector3 c;
    Vector4 uvst;  // Two texture coordinates, fit into one vec4 on the gpu.
    u16 joint_ids[4];
    f32 weights[4];
}
SAE_Vertex;
#define SAE_SKIN_VERTEX_FILESIZE sizeof(SAE_Skin_Vertex)

typedef struct SAE_Clip
{
    char name[64];
    f32 clip_length;  // seconds

    /// Array sizes: joint_count ///
    u32* joint_keyframe_counts;
    f32* joint_timestamps;
    Dual_Quaternion** joint_keyframes;
}
SAE_Clip;
#define SAE_CLIP_FILESIZE(joint_count) (68 + (joint_count) * 40))

typedef struct SAE_Material
{
    u32 map_count;  // max 3
    char (texture_paths[3])[256];  // length: map_count
}
SAE_Material;

typedef struct SAE_Rig_Header
{
    // Materials //
    u32 material_count;

    /// Skin ///
    u32 skinlet_count;
    struct {
        u32 skinlet_id;  // NOTE: Exporter should just add just an incrementing number as a unique id for skinlet
        u32 vertex_count;   // Allocate vertex_count * SAE_SKIN_VERTEX_SIZE
        u32 index_count;    // Allocate index_count * sizeof(u32)
        u32 material_id;
#define SAE_MAX_SKINLETS 32
    } skinlet_headers[SAE_MAX_SKINLETS];
    
    /// Skeleton ///
    u32 joint_count;

    /// Animation ///
    u32 clip_count;  // Allocate SAE_CLIP_FILESIZE(joint_count)

}
SAE_Rig_Header;
#define SAE_RIG_HEADER_FILESIZE(skinlet_count) 16 + SAE_MAX_SKINLETS * 16

/*
File format is:
SAE_Rig_Header (exactly as stated in struct)
-
SAE_Material* materials;

For each skinlet:
    u32 skinlet_id
    Skin_Vertex array,      length: vertex_count
    u32 indices array,      length: index_count

char[32] joint_name array,  length: joint_count (bytes: 32 * joint_count)
Clip array,             length: clip_count

*/

macro SAE_Rig_Header
sae_format_read_header(u8* file_buffer)
{
    u8* cursor = file_buffer;
    SAE_Rig_Header header = { 0 };
    header.material_count = ((u32*)cursor[0]);
    cursor += sizeof(u32);
    
    header.skinlet_counts = ((u32*)cursor)[0];
    copy_memory(cursor, sizeof(header.skinlet_headers), header.skinlet_headers);
    cursor += sizeof(header.skinlet_headers);
    header.joint_count = ((u32*)cursor)[0];
    header.clip_count = ((u32*)cursor)[1];

    return header;
}

// Store a default material texture
#define SAE_FORMAT_DEFAULT_MATERIAL { 1, { "game/data/textures/default_material.png", 0, 0 } };

#endif  // SAE_FORMAT_DESCRIPTION_H
