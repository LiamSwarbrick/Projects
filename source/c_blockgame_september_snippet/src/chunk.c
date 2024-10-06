#include "chunk.h"
#include "block.h"
#include <stdlib.h>
#include <string.h>
#include <GL/gl3w.h>
#include <GL/wglext.h>
#include <assert.h>
#include <cglm/cglm.h>


// Preallocated portion of memory for building chunk meshes (If threading is added these can't be globals)
f32* vertex_worker_buffer = NULL;
u32* index_worker_buffer = NULL;

// Increase their sizes if more complicated block meshes are added
// Size should be (number of blocks in chunk) * (size of largest block mesh type)
const u32 vertex_worker_buffer_size = CHUNK_VOLUME * NUM_CUBE_FACES * CUBE_FACE_NUM_VERTICES * VERTEX_FORMAT_NUM_FLOATS * sizeof(f32);
const u32 index_worker_buffer_size = CHUNK_VOLUME * 36 * sizeof(u32);  // 36 indices per block when culling is not used

// Chunk:
void
init_chunk_worker_buffers()
{
    vertex_worker_buffer = malloc(vertex_worker_buffer_size);
    index_worker_buffer = malloc(index_worker_buffer_size);
}

void
init_chunk(Chunk* c, s32 i, s32 j, s32 k)
{
    c->i = i;
    c->j = j;
    c->k = k;

    c->dirty = 1;
    c->face_count = 0;

    c->vertices_size = 0;
    c->indices_size = 0;

    // Init OpenGL VAO for chunk mesh
    glCreateVertexArrays(1, &c->vao);
    c->vbo = 0;  // Only allocate buffers when generating chunk meshes
    c->ebo = 0;
    // glCreateBuffers(1, &c->vbo);
    // glCreateBuffers(1, &c->ebo);

    // Allocate blocks
    c->blocks = malloc(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_LENGTH * sizeof(u8));

    // Init blocks to air
    for (int i = 0; i < CHUNK_VOLUME; ++i)
    {
        c->blocks[i] = BLOCK_air;
    }
}

void
free_chunk(Chunk* c)
{
    if (c)
    {
        free(c->blocks);

        glDeleteVertexArrays(1, &c->vao);
        glDeleteBuffers(1, &c->vbo);
        glDeleteBuffers(1, &c->ebo);

        c->blocks = NULL;
        c->vao = 0;
        c->vbo = 0;
        c->ebo = 0;
    }
}

b32
is_block_face_obstructed(Chunk* chunk, int x, int y, int z, int face, Chunk* neighbour_chunk)
{
    assert(face >= 0 && face < NUM_CUBE_FACES);

    vec3 face_normal = { face_normals[face][0], face_normals[face][1], face_normals[face][2] };
    
    vec3 pos = { (f32)x, (f32)y, (f32)z };
    vec3 neighbour_pos;
    glm_vec3_add(pos, face_normal, neighbour_pos);

    int nx = (int)neighbour_pos[0];
    int ny = (int)neighbour_pos[1];
    int nz = (int)neighbour_pos[2];
    u8 neighbour_block;

    if (nx >= 0 && nx < CHUNK_WIDTH &&
        ny >= 0 && ny < CHUNK_HEIGHT &&
        nz >= 0 && nz < CHUNK_LENGTH)
    {
        // Neighbour is within same chunk
        neighbour_block = BLOCK_AT(chunk->blocks, nx, ny, nz);
    }
    else if (neighbour_chunk)
    {
        // Adjust coordinates to wrap into neighbour chunk
        if (nx < 0) nx += CHUNK_WIDTH;
        if (ny < 0) ny += CHUNK_HEIGHT;
        if (nz < 0) nz += CHUNK_LENGTH;

        nx = nx % CHUNK_WIDTH;
        ny = ny % CHUNK_HEIGHT;
        nz = nz % CHUNK_LENGTH;

        neighbour_block = BLOCK_AT(neighbour_chunk->blocks, nx, ny, nz);
    }
    else
    {
        return 0;
    }

    // Return false if block is air or a non cube block
    if (neighbour_block == BLOCK_air)
    {
        return 0;
    }
    else if (block_templates[neighbour_block].mesh_type != MESH_CUBE)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void
gen_chunk_mesh(Chunk* c, Chunk_Map* map, b32 remesh_neighbours)
{
    assert(c);

    // Get neighbour chunks for face culling
    Chunk* chunk_neighbours[NUM_CUBE_FACES];
    for (int face = 0; face < NUM_CUBE_FACES; ++face)
    {
        Chunk* neighbour = find_chunk(map, c->i + (int)face_normals[face][0],
                                           c->j + (int)face_normals[face][1],
                                           c->k + (int)face_normals[face][2]);
        chunk_neighbours[face] = neighbour;

        // For optimal meshing we would also remesh the neighbouring chunks
        if (neighbour && remesh_neighbours)
        {
            gen_chunk_mesh(neighbour, map, 0);
        }
    }

    // Init mesh data
    memset(vertex_worker_buffer, 0, vertex_worker_buffer_size);
    memset(index_worker_buffer, 0, index_worker_buffer_size);

    c->dirty = 0;
    c->face_count = 0;

    c->vertices_size = 0;
    c->indices_size = 0;

    // For each block, add its visible faces
    for (int local_x = 0; local_x < CHUNK_WIDTH; ++local_x)
    {
        for (int local_y = 0; local_y < CHUNK_HEIGHT; ++local_y)
        {
            for (int local_z = 0; local_z < CHUNK_LENGTH; ++local_z)
            {
                u8 block_id = BLOCK_AT(c->blocks, local_x, local_y, local_z);
                if (block_id == BLOCK_air)
                    continue;

                Block_Template* block_template = &block_templates[block_id];

                // Calculate block position in world
                f32 x = (f32)(CHUNK_WIDTH  * c->i + local_x);
                f32 y = (f32)(CHUNK_HEIGHT * c->j + local_y);
                f32 z = (f32)(CHUNK_LENGTH * c->k + local_z);

                if (block_template->mesh_type == MESH_CUBE)
                {
                    for (int face = 0; face < NUM_CUBE_FACES; ++face)
                    {
                        if (is_block_face_obstructed(c, local_x, local_y, local_z, face, chunk_neighbours[face]))
                        {
                            continue;
                        }

                        // Copy face vertices to chunk mesh
                        u32 face_vertices_size = block_template->cube_face_size[face];
                        f32* vertices_dest = vertex_worker_buffer + c->vertices_size / sizeof(f32);

                        // Ensure we don't exceed buffer limits
                        if (c->vertices_size + face_vertices_size > vertex_worker_buffer_size) {
                            printf("Error: Chunk worker vertex buffer overflow.\n");
                            exit(1);
                        }

                        memcpy(vertices_dest, block_template->cube_face_vertices[face], face_vertices_size);
                        
                        // Translate position to blocks world position
                        for (int i = 0; i < face_vertices_size / sizeof(f32); i += VERTEX_FORMAT_NUM_FLOATS)
                        {
                            vertices_dest[i + 0] += x;
                            vertices_dest[i + 1] += y;
                            vertices_dest[i + 2] += z;
                        }

                        // Keep track of chunk mesh size as we go along
                        c->vertices_size += face_vertices_size;

                        // Copy face indices to chunk mesh
                        u32* indices_dest = index_worker_buffer + c->indices_size / sizeof(u32);

                        // Ensure we don't exceed buffer limits
                        if (c->indices_size + 6 * sizeof(u32) > index_worker_buffer_size) {
                            printf("Error: Chunk worker index buffer overflow.\n");
                            exit(1);
                        }

                        indices_dest[0] = 4 * c->face_count + 0;
                        indices_dest[1] = 4 * c->face_count + 1;
                        indices_dest[2] = 4 * c->face_count + 2;
                        indices_dest[3] = 4 * c->face_count + 0;
                        indices_dest[4] = 4 * c->face_count + 2;
                        indices_dest[5] = 4 * c->face_count + 3;
                        c->face_count += 1;
                        c->indices_size += 6 * sizeof(u32);
                    }
                }
                else
                {
                    // Different block mesh type
                }
            }
        }
    }
    

    if (c->vertices_size > 0 && c->indices_size > 0)
    {
        // Create OpenGL buffers
        glCreateBuffers(1, &c->vbo);
        glCreateBuffers(1, &c->ebo);

        // Upload to OpenGL
        glNamedBufferStorage(c->vbo, c->vertices_size, vertex_worker_buffer, GL_MAP_WRITE_BIT);
        glNamedBufferStorage(c->ebo, c->indices_size, index_worker_buffer, GL_MAP_WRITE_BIT);

        // attrib 0: Position (3D)
        glEnableVertexArrayAttrib(c->vao, 0);
        glVertexArrayAttribBinding(c->vao, 0, 0);
        glVertexArrayAttribFormat(c->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

        // attrib 1: Texture Coordinates (3D)
        glEnableVertexArrayAttrib(c->vao, 1);
        glVertexArrayAttribBinding(c->vao, 1, 0);
        glVertexArrayAttribFormat(c->vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32));

        // attrib 2: Shade Values (1D)
        glEnableVertexArrayAttrib(c->vao, 2);
        glVertexArrayAttribBinding(c->vao, 2, 0);
        glVertexArrayAttribFormat(c->vao, 2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(f32));

        // The vao's data lives in the vbo and ebo buffers which we uploaded to with glNamedBufferStorage before
        glVertexArrayVertexBuffer(c->vao, 0, c->vbo, 0, VERTEX_FORMAT_NUM_FLOATS * sizeof(f32));
        glVertexArrayElementBuffer(c->vao, c->ebo);
    }
}

// Chunk_Map:
void
init_chunk_map(Chunk_Map* map, u64 initial_capacity)
{
    map->entries = calloc(initial_capacity, sizeof(Chunk_Entry));
    map->count = 0;
    map->capacity = initial_capacity;
}

void
free_chunk_map(Chunk_Map* map)
{
    // NOTE: If chunks inside are not all freed this will leak all that memory
    free(map->entries);
}

u64
hash_chunk_position(s32 i, s32 j, s32 k)
{
    return (i * 73856093) ^ (j * 19349663) ^ (k * 83492791);
}

Chunk*
find_chunk(Chunk_Map* map, s32 i, s32 j, s32 k)
{
    u64 index = hash_chunk_position(i, j, k) % map->capacity;
    
    // Probe to end of array
    for (u64 id_i = index; id_i < map->capacity; ++id_i)
    {
        Chunk_Entry* entry = &map->entries[id_i];
        if (entry->chunk && entry->i == i && entry->j == j && entry->k == k)
        {
            return entry->chunk;
        }
    }
    
    // Wrap around and probe rest of entries
    for (u64 id_i = 0; id_i < index; ++id_i)
    {
        Chunk_Entry* entry = &map->entries[id_i];
        if (entry->chunk && entry->i == i && entry->j == j && entry->k == k)
        {
            return entry->chunk;
        }
    }

    return NULL;
}

void
add_chunk(Chunk_Map* map, Chunk* new_chunk)
{
    s32 i = new_chunk->i;
    s32 j = new_chunk->j;
    s32 k = new_chunk->k;

    u64 index = hash_chunk_position(i, j, k) % map->capacity;
    
    // Probe to find empty slot
    for (int id_i = index; id_i < map->capacity; ++id_i)
    {
        if (map->entries[id_i].chunk == NULL)
        {
            map->entries[id_i].i = i;
            map->entries[id_i].j = j;
            map->entries[id_i].k = k;
            map->entries[id_i].chunk = new_chunk;
            ++map->count;
            return;
        }
    }

    // Wrap around and continue probing all entries
    for (int id_i = 0; id_i < index; ++id_i)
    {
        if (map->entries[id_i].chunk == NULL)
        {
            map->entries[id_i].i = i;
            map->entries[id_i].j = j;
            map->entries[id_i].k = k;
            map->entries[id_i].chunk = new_chunk;
            ++map->count;
            return;
        }
    }

    // Map buffer is full, realloc with double the space
    u64 old_capacity = map->capacity;
    map->capacity *= 2;
    map->entries = realloc(map->entries, map->capacity * sizeof(Chunk_Entry));

    memset(map->entries + old_capacity, 0, old_capacity * sizeof(Chunk_Entry));

    add_chunk(map, new_chunk);
}

