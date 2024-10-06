#include "world.h"
#include "chunk.h"
#include <cglm/cglm.h>

void
load_near_chunks(Chunk_Map* map, vec3 player_pos, u32 chunk_load_distance)
{
    s32 player_chunk_x = (s32)player_pos[0] / CHUNK_WIDTH;
    s32 player_chunk_y = (s32)player_pos[1] / CHUNK_HEIGHT;
    s32 player_chunk_z = (s32)player_pos[2] / CHUNK_LENGTH;

    int dist = (int)chunk_load_distance;

    for (int i = player_chunk_x - dist; i <= player_chunk_x + dist; ++i)
    {
        for (int j = player_chunk_y - dist; j <= player_chunk_y + dist; ++j)
        {
            for (int k = player_chunk_z - dist; k <= player_chunk_z + dist; ++k)
            {
                if (!find_chunk(map, i, j, k))
                {
                    // Create new chunk and generate terrain
                    Chunk* new_chunk = (Chunk*)malloc(sizeof(Chunk));
                    init_chunk(new_chunk, i, j, k);
                    generate_chunk(new_chunk);
                    add_chunk(map, new_chunk);
                }
            }
        }
    }

}

void
unload_far_chunks(Chunk_Map* map, vec3 player_pos, u32 chunk_unload_distance)
{
    for (u64 i = 0; i < map->capacity; ++i)
    {
        Chunk* chunk = map->entries[i].chunk;
        if (chunk)
        {
            // Unload distant chunks
            {
                int dist_x = abs(chunk->i - (int)player_pos[0] / CHUNK_WIDTH);
                int dist_y = abs(chunk->j - (int)player_pos[1] / CHUNK_HEIGHT);
                int dist_z = abs(chunk->k - (int)player_pos[2] / CHUNK_LENGTH);
                if (dist_x > chunk_unload_distance ||
                    dist_y > chunk_unload_distance ||
                    dist_z > chunk_unload_distance)
                {
                    free_chunk(chunk);
                    map->entries[i].chunk = NULL;
                }
            }
        }
    }
}

void
generate_chunk(Chunk* c)
{
    if (c->i + c->j + c->k > 0)
        gen_sphere(c);
    else
    {
        int world_x, world_y, world_z;
        for (int x = 0; x < CHUNK_WIDTH; ++x)
        {
            world_x = (CHUNK_WIDTH * c->i) + x;
            for (int y = 0; y < CHUNK_HEIGHT; ++y)
            {
                world_y = (CHUNK_HEIGHT * c->j) + y;
                for (int z = 0; z < CHUNK_LENGTH; ++z)
                {
                    world_z = (CHUNK_LENGTH * c->k) + z;

                    int middle = world_x + world_y + world_z;
                    if (world_y <= 0)
                        middle = world_y;  // flatten world at 0
                    
                    if (middle > 0)
                        BLOCK_AT(c->blocks, x, y, z) = BLOCK_air;
                    else if (middle == 0)
                        BLOCK_AT(c->blocks, x, y, z) = BLOCK_grass;
                    else if (middle >= -8)
                        BLOCK_AT(c->blocks, x, y, z) = BLOCK_dirt;
                    else if (middle < -8)
                        BLOCK_AT(c->blocks, x, y, z) = BLOCK_stone;
                }
            }
        }
    }
}

void
gen_sphere(Chunk* c)
{
    for (int x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int z = 0; z < CHUNK_LENGTH; ++z)
            {
                int x0 = CHUNK_WIDTH / 2;
                int y0 = CHUNK_HEIGHT / 2;
                int z0 = CHUNK_LENGTH / 2;
                float dist = sqrt((double)((x-x0)*(x-x0)) + (double)((y-y0)*(y-y0)) + (double)((z-z0)*(z-z0)));
                if (dist < (CHUNK_WIDTH/2))
                    BLOCK_AT(c->blocks, x, y, z) = BLOCK_oak_leaf;
            }
        }
    }
}
