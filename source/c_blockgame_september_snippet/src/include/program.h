#ifndef BLOCKGAME_PROGRAM_H
#define BLOCKGAME_PROGRAM_H

#include <windows.h>
#include <cglm/cglm.h>
// Undefine winapi's DOGWATER defines
#undef max
#undef min

#include "basic_types.h"
#include "chunk.h"

#define WINDOW_TITLE "Blockgame"
#define WINDOW_INITIAL_W 1200
#define WINDOW_INITIAL_H 900

typedef struct Program
{
    // Window
    u32 w, h;
    f32 aspect_ratio;
    HWND window_handle;  // Windows OS
    b32 is_initialised;
    u32 target_fps;
    f64 time;  // seconds
    f32 dt;    // seconds

    // Settings
    b32 render_as_wireframe;
    u32 chunk_load_distance;  // Possibility to have chunk_render_distance as a seperate variable later
    u32 chunk_unload_distance;
    f32 fov_y;

    // OpenGL
    u32 block_texture_array;
    u32 chunk_shader;

    // Game Map
    Chunk_Map map;
}
Program;

typedef struct Input
{
    // Mouse:
    b32 mouse_fps_mode;  // First-Person Shooter style mouse capture
    s32 mouse_pos_x;
    s32 mouse_pos_y;
    s32 mouse_relative_x;
    s32 mouse_relative_y;
    s32 mouse_fps_pivot_x;
    s32 mouse_fps_pivot_y;

    // Keyboard:
    b8 keys[0xFF];
    b8 last_keys[0xFF];
}
Input;

extern Program program;
extern Input input, last_input;

void init();
void update_and_render();

#endif  // BLOCKGAME_PROGRAM_H
