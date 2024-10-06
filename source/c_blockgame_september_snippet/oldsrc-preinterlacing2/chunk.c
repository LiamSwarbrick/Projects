#include "chunk.h"
#include <stdlib.h>
#include <string.h>
#include <GL/gl3w.h>
#include <GL/wglext.h>


f32* positions_buffer = NULL;
f32* tex_coords_buffer = NULL;
f32* shade_values_buffer = NULL;
u32* indices_buffer = NULL;


void
setup_chunk_arenas()
{
    // Allocate amply sized reusable buffers for chunk uploading
    const int chunk_volume = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_LENGTH;
    const int max_vertices_per_block = 36;

    positions_buffer     = malloc(chunk_volume * max_vertices_per_block * 3 * sizeof(f32));
    tex_coords_buffer    = malloc(chunk_volume * max_vertices_per_block * 3 * sizeof(f32));
    shade_values_buffer = malloc(chunk_volume * max_vertices_per_block * 1 * sizeof(f32));
    indices_buffer = malloc(chunk_volume * max_vertices_per_block * 1 * sizeof(u32));
}

void
init_chunk(Chunk* c, int i, int j, int k)
{
    c->i = i;
    c->j = j;
    c->k = k;

    c->has_mesh = 0;
    c->index_counter = 0;

    // Init OpenGL buffers for chunk mesh
    glCreateVertexArrays(1, &c->vao);
    glCreateBuffers(1, &c->vbo_positions);
    glCreateBuffers(1, &c->vbo_tex_coords);
    glCreateBuffers(1, &c->vbo_shade_values);
    glCreateBuffers(1, &c->ebo);

    // Allocate blocks
    c->blocks = malloc(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_LENGTH * sizeof(u8));

    // Init blocks to air
    for (int local_x = 0; local_x < CHUNK_WIDTH; ++local_x)
    {
        for (int local_y = 0; local_y < CHUNK_HEIGHT; ++local_y)
        {
            for (int local_z = 0; local_z < CHUNK_LENGTH; ++local_z)
            {
                c->blocks[BLOCK_INDEX(local_x, local_y, local_z)] = BLOCK_air;
            }
        }
    }
}

void
free_chunk(Chunk* c)
{
    if (c)
    {
        free(c->blocks);

        glDeleteVertexArrays(1, &c->vao);
        glDeleteBuffers(1, &c->vbo_positions);
        glDeleteBuffers(1, &c->vbo_tex_coords);
        glDeleteBuffers(1, &c->vbo_shade_values);
        glDeleteBuffers(1, &c->ebo);

        c->blocks = NULL;
        c->vao = 0;
        c->vbo_positions = 0;
        c->vbo_tex_coords = 0;
        c->vbo_shade_values = 0;
        c->ebo = 0;
    }
}

void
update_chunk(Chunk* c)
{
    c->has_mesh = 1;
    c->index_counter = 0;
    c->positions_size = 0;
    c->tex_coords_size = 0;
    c->shade_values_size = 0;
    c->indices_size = 0;

    for (int local_x = 0; local_x < CHUNK_WIDTH; ++local_x)
    {
        for (int local_y = 0; local_y < CHUNK_HEIGHT; ++local_y)
        {
            for (int local_z = 0; local_z < CHUNK_LENGTH; ++local_z)
            {
                printf("%d, %d, %d: %d / %d\n", local_x, local_y, local_z, BLOCK_INDEX(local_x, local_y, local_z), CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_LENGTH);
                // Calculate block position
                f32 x = (f32)(c->i + local_x);
                f32 y = (f32)(c->j + local_y);
                f32 z = (f32)(c->k + local_z);

                // Retrive block template
                Block_Definition* block = &block_defs[c->blocks[BLOCK_INDEX(local_x, local_y, local_z)]];
                
                for (int i = 0; i < NUM_FACES; ++i)
                {
                    // TODO: if face is visible {
                    // memcpy(positions_buffer + c->positions_size, block->positions[i], block->positions_sizes[i]);
                    for (int j = 0; j < block->positions_sizes[i]; j += 3)
                    {
                        // Translate block positions
                        printf("& = %d\n", &positions_buffer[c->positions_size + j] - positions_buffer);
                        positions_buffer[c->positions_size + j]     = block->positions[i][j]     + x;
                        positions_buffer[c->positions_size + j + 1] = block->positions[i][j + 1] + y;
                        positions_buffer[c->positions_size + j + 2] = block->positions[i][j + 2] + z;
                    }
                    c->positions_size += block->positions_sizes[i];

                    memcpy(tex_coords_buffer + c->tex_coords_size, block->tex_coords[i], block->tex_coords_sizes[i]);
                    c->tex_coords_size += block->tex_coords_sizes[i];

                    memcpy(shade_values_buffer + c->shade_values_size, block->shade_values[i], block->shade_values_sizes[i]);
                    c->shade_values_size += block->shade_values_sizes[i];

                    indices_buffer[c->index_counter + 0] = c->index_counter + 0;
                    indices_buffer[c->index_counter + 1] = c->index_counter + 1;
                    indices_buffer[c->index_counter + 2] = c->index_counter + 2;
                    indices_buffer[c->index_counter + 3] = c->index_counter + 0;
                    indices_buffer[c->index_counter + 4] = c->index_counter + 2;
                    indices_buffer[c->index_counter + 5] = c->index_counter + 3;
                    c->index_counter += 6;
                    // }
                }
            }
        }
    }

    c->indices_size = c->index_counter * sizeof(u32);

    // Upload to OpenGL
    glNamedBufferStorage(c->vbo_positions, c->positions_size, positions_buffer, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(c->vbo_tex_coords, c->tex_coords_size, tex_coords_buffer, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(c->vbo_shade_values, c->shade_values_size, shade_values_buffer, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(c->ebo, c->indices_size, indices_buffer, GL_MAP_WRITE_BIT);

    // Position attribute (0)
    glEnableVertexArrayAttrib(c->vao, 0);
    glVertexArrayAttribBinding(c->vao, 0, 0);
    glVertexArrayAttribFormat(c->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(c->vao, 0, c->vbo_positions, 0, 3 * sizeof(f32));

    // UV attribute (1)
    glEnableVertexArrayAttrib(c->vao, 1);
    glVertexArrayAttribBinding(c->vao, 1, 1);
    glVertexArrayAttribFormat(c->vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(c->vao, 1, c->vbo_tex_coords, 0, 3 * sizeof(f32));

    // Shade attribute (2)
    glEnableVertexArrayAttrib(c->vao, 2);
    glVertexArrayAttribBinding(c->vao, 2, 2);
    glVertexArrayAttribFormat(c->vao, 2, 1, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(c->vao, 2, c->vbo_shade_values, 0, 1 * sizeof(f32));

    // Element buffer
    glVertexArrayElementBuffer(c->vao, c->ebo);
}

