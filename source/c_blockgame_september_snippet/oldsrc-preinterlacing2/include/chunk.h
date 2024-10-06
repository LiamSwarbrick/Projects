#ifndef BLOCKGAME_CHUNK_H
#define BLOCKGAME_CHUNK_H

#include "basic_types.h"
#include "block.h"
#include <cglm/cglm.h>

#define CHUNK_WIDTH  8//32
#define CHUNK_HEIGHT 8//32
#define CHUNK_LENGTH 8//32

// Macro for getting index for flat Chunk.blocks array from x,y,z
#define BLOCK_INDEX(local_x, local_y, local_z) (CHUNK_LENGTH * ((local_x) * CHUNK_HEIGHT + (local_y)) + (local_z))

typedef struct Chunk
{
    s32 i, j, k;  // Chunk position

    int has_mesh;  // else requires new mesh
    u32 index_counter;

    u32 positions_size;
    u32 tex_coords_size;
    u32 shade_values_size;
    u32 indices_size;

    u32 vao;
    u32 vbo_positions;
    u32 vbo_tex_coords;
    u32 vbo_shade_values;
    u32 ebo;

    u8* blocks;
}
Chunk;

// static vec3 chunk_world_pos(Chunk* chunk)
// {
//     return (vec3){ chunk->i * CHUNK_WIDTH, chunk->j * CHUNK_HEIGHT, chunk->k * CHUNK_LENGTH };
// }

void setup_chunk_arenas();
void init_chunk(Chunk* c, int i, int j, int k);
void update_chunk(Chunk* c);

#endif  // BLOCKGAME_CHUNK_H
