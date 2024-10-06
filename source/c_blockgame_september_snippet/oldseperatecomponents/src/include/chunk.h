#ifndef BLOCKGAME_CHUNK_H
#define BLOCKGAME_CHUNK_H

#include "basic.h"
#include "block.h"

#define CHUNK_WIDTH 32
#define CHUNK_LENGTH 32
#define CHUNK_HEIGHT 32

typedef struct Block
{
    int x, y, z;
    int type;
}
Block;

typedef struct Chunk
{
    s32 i,j,k;  // chunk space position
    b32 is_dirty;  // needs mesh (term from memory paging)
    
}
Chunk;



#endif  // BLOCKGAME_CHUNK_H
