#ifndef BLOCKGAME_WORLD_H
#define BLOCKGAME_WORLD_H

#include "basic_types.h"
#include "chunk.h"

typedef struct World_Metadata
{
    // TODO: World should have name, generation parameters, etc...
}
World_Metadata;

void load_near_chunks(Chunk_Map* map, vec3 player_pos, u32 chunk_load_distance);
void unload_far_chunks(Chunk_Map* map, vec3 player_pos, u32 chunk_unload_distance);
void generate_chunk(Chunk* c);

#endif  // BLOCKGAME_WORLD_H
