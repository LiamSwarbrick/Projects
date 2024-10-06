#ifndef BLOCKGAME_CHUNK_H
#define BLOCKGAME_CHUNK_H

#include "basic_types.h"
#include "block.h"
#include <cglm/cglm.h>

#define CHUNK_WIDTH  32
#define CHUNK_HEIGHT 32
#define CHUNK_LENGTH 32
#define CHUNK_VOLUME (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_LENGTH)

// Macro for indexing into flat Chunk.blocks array from x,y,z
#define BLOCK_AT(blocks, local_x, local_y, local_z) blocks[(CHUNK_LENGTH * ((local_x) * CHUNK_HEIGHT + (local_y)) + (local_z))]

#define FOR_EACH_BLOCK(x0,y0,z0, x1,y1,z1) \
        for (int x = x0; x < x1; ++x) \
        for (int y = y0; y < y1; ++y) \
        for (int z = z0; z < z1; ++z)

typedef struct Chunk
{
    s32 i, j, k;  // Chunk position

    b32 dirty;  // requires new mesh
    u32 face_count;

    u32 vertices_size;
    u32 indices_size;

    u32 vao;
    u32 vbo;
    u32 ebo;

    u8* blocks;
}
Chunk;

typedef struct Chunk_Entry
{
    s32 i, j, k;  // Chunk position
    Chunk* chunk;
}
Chunk_Entry;

typedef struct Chunk_Map
{
    Chunk_Entry* entries;
    u64 count;  // just for keeping track of how many chunks are actually loaded
    u64 capacity;
}
Chunk_Map;


// static vec3 chunk_world_pos(Chunk* chunk)
// {
//     return (vec3){ chunk->i * CHUNK_WIDTH, chunk->j * CHUNK_HEIGHT, chunk->k * CHUNK_LENGTH };
// }

// Chunk:
void init_chunk_worker_buffers();
void init_chunk(Chunk* c, s32 i, s32 j, s32 k);
void free_chunk(Chunk* c);
// b32 is_face_obstructed(int x, int y, int z, int face, Chunk* current_chunk, Chunk* border);
b32 is_block_face_obstructed(Chunk* chunk, int x, int y, int z, int face, Chunk* neighbour_chunk);
void gen_chunk_mesh(Chunk* c, Chunk_Map* map, b32 remesh_neighbours);

// Chunk_Map:
void init_chunk_map(Chunk_Map* map, u64 initial_capacity);
void free_chunk_map(Chunk_Map* map);
u64 hash_chunk_position(s32 i, s32 j, s32 k);
Chunk* find_chunk(Chunk_Map* map, s32 i, s32 j, s32 k);
void add_chunk(Chunk_Map* map, Chunk* new_chunk);

#endif  // BLOCKGAME_CHUNK_H
