#ifndef BLOCKGAME_BLOCK_PREFABS_H
#define BLOCKGAME_BLOCK_PREFABS_H

#include "basic_types.h"
#include <cglm/cglm.h>

#define CUBE_FACE_NUM_VERTICES 4
#define VERTEX_FORMAT_NUM_FLOATS 7

typedef enum
{
    BLOCK_air=0,
    BLOCK_grass,
    BLOCK_dirt,
    BLOCK_stone,
    BLOCK_cobblestone,
    BLOCK_oak_log,
    BLOCK_oak_leaf,
    BLOCK_oak_plank,
    BLOCK_sand,
    // BLOCK_red_flower,
    // BLOCK_yellow_flower,

    BLOCK_TYPE_COUNT
}
Block_ID;

typedef enum
{
    FACE_T=0,     // Top (0, 1, 0)
    FACE_B,       // Bottom
    FACE_N,       // North=Forward=(0, 0, -1)
    FACE_S,       // South
    FACE_E,       // East=Right=(1, 0, 0)
    FACE_W,       // West
    NUM_CUBE_FACES
}
Face_Indices;

static const vec3 face_normals[NUM_CUBE_FACES] = {
    {  0.0f,  1.0f,  0.0f },  // T
    {  0.0f, -1.0f,  0.0f },  // B
    {  0.0f,  0.0f, -1.0f },  // N
    {  0.0f,  0.0f,  1.0f },  // S
    {  1.0f,  0.0f,  0.0f },  // E
    { -1.0f,  0.0f,  0.0f }   // W
};

typedef enum
{
    NO_MESH=0,
    MESH_CUBE,
}
Block_Mesh_Types;


typedef struct Block_Template
{
    char name[64];
    b32 has_collision;
    u32 mesh_type;

    u32 cube_face_size[NUM_CUBE_FACES];
    f32* cube_face_vertices[NUM_CUBE_FACES];

    // Extend by using mesh_type and then superstruct with
    // cube_vertices[6];
    // x_mesh_vertices[2];
    // ...
}
Block_Template;

extern Block_Template block_templates[BLOCK_TYPE_COUNT];  // Defined in block.c

void define_blocks_templates();

#endif  // BLOCKGAME_BLOCK_PREFABS_H
