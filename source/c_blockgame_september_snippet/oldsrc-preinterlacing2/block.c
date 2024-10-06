#include "block.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

Block_Definition block_defs[BLOCK_TYPE_COUNT];


const f32 cube_vertex_positions[NUM_FACES][NUM_VERTICES_PER_FACE * 3] = {
    { -0.5f,  0.5f,  0.5f,   0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f }, // Top face
    { -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f }, // Bottom face
    { -0.5f,  0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f }, // North face
    {  0.5f,  0.5f, -0.5f,   0.5f, -0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f }, // South face
    {  0.5f,  0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f }, // East face
    { -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f }, // West face
};

const f32 cube_tex_coords[NUM_FACES][NUM_VERTICES_PER_FACE * 3] = {
    { 0,0,0,  1,0,0,  1,1,0,  0,1,0 }, // Top face
    { 0,0,0,  1,0,0,  1,1,0,  0,1,0 }, // Bottom face
    { 0,1,0,  0,0,0,  1,0,0,  1,1,0 }, // North face
    { 1,1,0,  1,0,0,  0,0,0,  0,1,0 }, // South face
    { 1,1,0,  1,0,0,  0,0,0,  0,1,0 }, // East face
    { 0,1,0,  0,0,0,  1,0,0,  1,1,0 }, // West face
};

const f32 cube_shade_values[NUM_FACES][NUM_VERTICES_PER_FACE * 1] = {
    { 1.0f, 1.0f, 1.0f, 1.0f }, // Top face
    { 0.5f, 0.5f, 0.5f, 0.5f }, // Bottom face
    { 0.7f, 0.7f, 0.7f, 0.7f }, // North face
    { 0.8f, 0.8f, 0.8f, 0.8f }, // South face
    { 0.6f, 0.6f, 0.6f, 0.6f }, // East face
    { 0.6f, 0.6f, 0.6f, 0.6f }, // West face
};

// const u32 cube_face_indices[NUM_FACES][6] = {
//     { 0,1,2, 0,2,3 },  // Top face
//     { 4,5,6, 4,6,7 },  // Bottom face
//     { 8,9,10, 8,10,11 },  // North face
//     { 12,13,14, 12,14,15 },  // South face
//     { 16,17,18, 16,18,19 },  // East face
//     { 20,21,22, 20,19,21 },  // West face
// };


// const f32 x_mesh_vertex_positions[] = {
//     -0.5f, 0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
//     -0.5f, 0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,
// };

// const f32 x_mesh_tex_coords[] = {
//     0,1,0, 0,0,0, 1,0,0, 1,1,0,
//     0,1,0, 0,0,0, 1,0,0, 1,1,0,
// };

// const f32 x_mesh_shade_values[] =  {
//     0.7f, 0.7f, 0.7f, 0.7f,
//     0.7f, 0.7f, 0.7f, 0.7f,
// };

// const f32 x_mesh_indices[] = {
//     // Counter clockwise winding:
//     0, 1, 2,  0, 2, 3,
//     4, 5, 6,  4, 6, 7,
//     // Clockwise winding:
//     2, 1, 0,  3, 2, 0,
//     6, 5, 4,  7, 6, 4,
// };

Block_Definition
define_cube_block(const char* name, int T_tex, int B_tex,
                                    int N_tex, int S_tex,
                                    int E_tex, int W_tex)
{    
    Block_Definition b;

    // Copy over the name
    assert(strlen(name) < 64);
    strcpy(b.name, name);

    // Cubes have collision
    b.has_collision = 1;

    // Texture layers for each face
    int tex_layers[NUM_FACES] = { T_tex, B_tex, N_tex, S_tex, E_tex, W_tex };

    // Copy the vertex positions, texture coordinates, and shade values for each face
    for (int face = 0; face < NUM_FACES; ++face)
    {
        b.positions_sizes[face]    = NUM_VERTICES_PER_FACE * 3 * sizeof(f32);
        b.tex_coords_sizes[face]   = NUM_VERTICES_PER_FACE * 3 * sizeof(f32);
        b.shade_values_sizes[face] = NUM_VERTICES_PER_FACE * 1 * sizeof(f32);

        // Allocate memory for positions, texture coordinates, and shade values for each face
        b.positions[face]    = malloc(b.positions_sizes[face]);
        b.tex_coords[face]   = malloc(b.tex_coords_sizes[face]);
        b.shade_values[face] = malloc(b.shade_values_sizes[face]);

        // Copy vertex positions
        memcpy(b.positions[face], cube_vertex_positions[face], b.positions_sizes[face]);

        // Copy texture coordinates and set texture layer
        for (int vertex = 0; vertex < NUM_VERTICES_PER_FACE; ++vertex)
        {
            b.tex_coords[face][vertex * 3 + 0] = cube_tex_coords[face][vertex * 3 + 0];
            b.tex_coords[face][vertex * 3 + 1] = cube_tex_coords[face][vertex * 3 + 1];
            b.tex_coords[face][vertex * 3 + 2] = (f32)tex_layers[face];
        }

        // Copy shade values
        memcpy(b.shade_values[face], cube_shade_values[face], b.shade_values_sizes[face]);
    }

    return b;

    // // Copy the vertex positions
    // b.positions = malloc(sizeof(cube_vertex_positions));
    // memcpy(b.positions, cube_vertex_positions, sizeof(cube_vertex_positions));
    // b.positions_size = sizeof(cube_vertex_positions);

    // // Copy the texture coordinates
    // b.tex_coords = malloc(sizeof(cube_tex_coords));
    // memcpy(b.tex_coords, cube_tex_coords, sizeof(cube_tex_coords));
    // b.tex_coords_size = sizeof(cube_tex_coords);

    // // Copy the shade values
    // b.shade_values = malloc(sizeof(cube_shade_values));
    // memcpy(b.shade_values, cube_shade_values, sizeof(cube_shade_values));
    // b.shade_values_size = sizeof(cube_shade_values);

    // // Copy the indices
    // b.indices = malloc(sizeof(cube_indices));
    // memcpy(b.indices, cube_indices, sizeof(cube_indices));
    // b.indices_size = sizeof(cube_indices);

    // // Set textures for each face (using the array texture layers)
    // for (int vertex = 0; vertex < 4; ++vertex)
    // {
    //     b.tex_coords[FACE_T * 12 + vertex * 3 + 2] = T_tex;
    //     b.tex_coords[FACE_B * 12 + vertex * 3 + 2] = B_tex;
    //     b.tex_coords[FACE_N * 12 + vertex * 3 + 2] = N_tex;
    //     b.tex_coords[FACE_S * 12 + vertex * 3 + 2] = S_tex;
    //     b.tex_coords[FACE_E * 12 + vertex * 3 + 2] = E_tex;
    //     b.tex_coords[FACE_W * 12 + vertex * 3 + 2] = W_tex;
    // }

    // return b;
}

void
define_blocks()
{
    block_defs[BLOCK_grass] = define_cube_block("Grass", 0,2,1,1,1,1);
    block_defs[BLOCK_dirt] = define_cube_block("Dirt", 2,2,2,2,2,2);
    block_defs[BLOCK_stone] = define_cube_block("Stone", 3,3,3,3,3,3);
    block_defs[BLOCK_cobblestone] = define_cube_block("Cobblestone", 4,4,4,4,4,4);
    block_defs[BLOCK_oak_log] = define_cube_block("Oak Log", 6,6,5,5,5,5);
    block_defs[BLOCK_oak_leaf] = define_cube_block("Oak Leaf", 7,7,7,7,7,7);
    block_defs[BLOCK_oak_plank] = define_cube_block("Oak Plank", 8,8,8,8,8,8);
    block_defs[BLOCK_sand] = define_cube_block("Sand", 9,9,9,9,9,9);
    // block_defs[BLOCK_red_flower] = define_x_mesh_block("Red flower", 10);
    // block_defs[BLOCK_yellow_flower] = define_x_mesh_block("Yellow flower", 11);
}
