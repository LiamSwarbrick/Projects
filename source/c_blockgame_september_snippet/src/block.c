#include "block.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

Block_Template block_templates[BLOCK_TYPE_COUNT];

// Vertex format: position (3D), texcoords (3D), shade_value (1D)
const f32 cube_face_vertices[NUM_CUBE_FACES][CUBE_FACE_NUM_VERTICES * VERTEX_FORMAT_NUM_FLOATS] = {
    // Top face
    {
        -0.5f,  0.5f,  0.5f,  0,0,0,  1.0f,
         0.5f,  0.5f,  0.5f,  1,0,0,  1.0f,
         0.5f,  0.5f, -0.5f,  1,1,0,  1.0f,
        -0.5f,  0.5f, -0.5f,  0,1,0,  1.0f
    },
    // Bottom face
    {
        -0.5f, -0.5f, -0.5f,  0,0,0,  0.5f,
         0.5f, -0.5f, -0.5f,  1,0,0,  0.5f,
         0.5f, -0.5f,  0.5f,  1,1,0,  0.5f,
        -0.5f, -0.5f,  0.5f,  0,1,0,  0.5f
    },
    // North face
    {
         0.5f,  0.5f, -0.5f,  1,1,0,  0.8f,
         0.5f, -0.5f, -0.5f,  1,0,0,  0.8f,
        -0.5f, -0.5f, -0.5f,  0,0,0,  0.8f,
        -0.5f,  0.5f, -0.5f,  0,1,0,  0.8f
    },
    // South face
    {
        -0.5f,  0.5f,  0.5f,  0,1,0,  0.7f,  
        -0.5f, -0.5f,  0.5f,  0,0,0,  0.7f,
         0.5f, -0.5f,  0.5f,  1,0,0,  0.7f,
         0.5f,  0.5f,  0.5f,  1,1,0,  0.7f
    },
    // East face
    {
         0.5f,  0.5f,  0.5f,  1,1,0,  0.6f,  
         0.5f, -0.5f,  0.5f,  1,0,0,  0.6f,
         0.5f, -0.5f, -0.5f,  0,0,0,  0.6f,
         0.5f,  0.5f, -0.5f,  0,1,0,  0.6f
    },
    // West face
    {
        -0.5f,  0.5f, -0.5f,  0,1,0,  0.6f,
        -0.5f, -0.5f, -0.5f,  0,0,0,  0.6f,
        -0.5f, -0.5f,  0.5f,  1,0,0,  0.6f,
        -0.5f,  0.5f,  0.5f,  1,1,0,  0.6f
    },
};


Block_Template
define_cube_block(const char* name, int T_tex, int B_tex,
                                    int N_tex, int S_tex,
                                    int E_tex, int W_tex)
{
    Block_Template b;

    // Copy over the name
    assert(strlen(name) < 64);
    strcpy(b.name, name);

    // Cubes have collision
    b.has_collision = 1;
    b.mesh_type = MESH_CUBE;

    // Texture layers for each face
    int tex_layers[NUM_CUBE_FACES] = { T_tex, B_tex, N_tex, S_tex, E_tex, W_tex };

    // Copy the vertex positions, texture coordinates, and shade values for each face
    for (int face = 0; face < NUM_CUBE_FACES; ++face)
    {
        b.cube_face_size[face] = CUBE_FACE_NUM_VERTICES * VERTEX_FORMAT_NUM_FLOATS * sizeof(f32);

        // Allocate memory for each face's vertices
        b.cube_face_vertices[face] = malloc(b.cube_face_size[face]);

        // Copy vertex data
        memcpy(b.cube_face_vertices[face], cube_face_vertices[face], b.cube_face_size[face]);

        // Copy texture coordinates and set texture layer
        for (int vertex = 0; vertex < CUBE_FACE_NUM_VERTICES; ++vertex)
        {
            b.cube_face_vertices[face][vertex * VERTEX_FORMAT_NUM_FLOATS + 5] = (f32)tex_layers[face];
        }
    }

    return b;
}

void
define_blocks_templates()
{
    block_templates[BLOCK_grass]            = define_cube_block("Grass",        0,2,1,1,1,1);
    block_templates[BLOCK_dirt]             = define_cube_block("Dirt",         2,2,2,2,2,2);
    block_templates[BLOCK_stone]            = define_cube_block("Stone",        3,3,3,3,3,3);
    block_templates[BLOCK_cobblestone]      = define_cube_block("Cobblestone",  4,4,4,4,4,4);
    block_templates[BLOCK_oak_log]          = define_cube_block("Oak Log",      6,6,5,5,5,5);
    block_templates[BLOCK_oak_leaf]         = define_cube_block("Oak Leaf",     7,7,7,7,7,7);
    block_templates[BLOCK_oak_plank]        = define_cube_block("Oak Plank",    8,8,8,8,8,8);
    block_templates[BLOCK_sand]             = define_cube_block("Sand",         9,9,9,9,9,9);
    // block_defs[BLOCK_red_flower] = define_x_mesh_block("Red flower", 10);
    // block_defs[BLOCK_yellow_flower] = define_x_mesh_block("Yellow flower", 11);
}
