#ifndef BLOCKGAME_PROGRAM_H
#define BLOCKGAME_PROGRAM_H

#include <windows.h>
#include "basic_types.h"

#define WINDOW_TITLE "Blockgame"
#define WINDOW_INITIAL_W 600//1200
#define WINDOW_INITIAL_H 600//900

typedef struct Program
{
    u32 w, h;
    f32 aspect_ratio;
    HWND window_handle;  // Windows OS
    b32 is_initialised;
    u32 target_fps;
    f64 time;  // seconds
    f32 dt;    // seconds

    u32 vao, vbo, ebo;
    u32 tex;
    u32 shader_program;
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
