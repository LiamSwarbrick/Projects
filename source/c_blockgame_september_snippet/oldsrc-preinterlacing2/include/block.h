#ifndef BLOCKGAME_BLOCK_PREFABS_H
#define BLOCKGAME_BLOCK_PREFABS_H

#include "basic_types.h"

#define NUM_FACES 6
#define NUM_VERTICES_PER_FACE 4

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
}
Face_Indices;

typedef struct Block_Definition
{
    char name[64];
    b32 has_collision;

    u32 positions_sizes[6];
    u32 tex_coords_sizes[6];
    u32 shade_values_sizes[6];

    f32* positions[6];
    f32* tex_coords[6];
    f32* shade_values[6];

    // Extend with:
    // u32 shape_type;  // SHAPE_CUBE, SHAPE_X_MESH, SHAPE_BED etc...
    // Then super struct
    // cube_positions[6];
    // ...
    // x_mesh_positions[2];
    // ...
}
Block_Definition;

extern Block_Definition block_defs[BLOCK_TYPE_COUNT];  // Defined in block.c

void define_blocks();

#endif  // BLOCKGAME_BLOCK_PREFABS_H
