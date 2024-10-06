#ifndef BLOCKGAME_BLOCK_PREFABS_H
#define BLOCKGAME_BLOCK_PREFABS_H

#include "basic_types.h"

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
    BLOCK_red_flower,
    BLOCK_yellow_flower,

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
    FACE_dir_count//=6
}
Face_Indices;

typedef struct Block_Definition
{
    char name[64];
    b32 has_collision;

    u32 positions_size, tex_coords_size, shade_values_size, indices_size;
    f32* positions;
    f32* tex_coords;
    f32* shade_values;
    u32* indices;
}
Block_Definition;

extern Block_Definition block_defs[BLOCK_TYPE_COUNT];  // Defined in block.c

extern const f32 cube_vertex_positions[];
extern const f32 cube_tex_coords[];
extern const u32 cube_indices[];

void define_blocks();

#endif  // BLOCKGAME_BLOCK_PREFABS_H
