#ifndef SK_H
#define SK_H

#include "common_types.h"
#include "memory_arena.h"
#include "input.h"
#include "opengl_layer_types.h"

typedef struct Platform
{
    // NOTE: Game state things (necessary to describe game state)
    void*  permanent_storage;
    size_t permanent_storage_size;
    // NOTE: Game assets (permanent while game is running, but not necessary to describe game state)
    void*  asset_storage;
    size_t asset_storage_size;
    // NOTE: Scratch pad for temp memory like string manip, file loading parsing things
    void*  transient_storage;
    size_t transient_storage_size;

#define WINDOW_TITLE_MAX_CHARS 256
    char window_title[WINDOW_TITLE_MAX_CHARS];
    b32 window_should_update_title;  // NOTE: Set back to 0 after changed
    int window_width;
    int window_height;
    f32 aspect_ratio;
    b32 quit;
    b32 initialized;

    b32 fullscreen;
    b32 vsync;
    b32 cursor_visible;
    s32 texture_filter;

    f32 target_fps;
    u64 timems;
    f64 time;
    s64 previous_timems;
    f64 previous_time;
    s64 delta_timems;
    f64 delta_time;
    
    Input input;
    Input last_input;

    void (*resize_callback)(int width, int height);
}
Platform;


// NOTE: Platform layer implements these:

void* DEBUG_read_entire_file(char* filename);
void DEBUG_free_file_memory(void* memory);
b32 DEBUG_write_entire_file(char* filename, u32 size, void* memory);

void set_window_title(char* text);
void output_debug_string(char* text);
void report_error(char* text);
void swap_buffers();

typedef struct Game
{
    Memory_Arena permanent_arena;  // NOTE: Permanent state, Top arena for permanent_storage
    Memory_Arena scene_arena;      // NOTE: Entity states, in permanent_arena
    Memory_Arena asset_arena;      // NOTE: Assets, in asset_storage
    Memory_Arena frame_arena;      // NOTE: Emptied every frame, Top arena for transient_storage
    
    Shader shader;
    Texture2D tnt_texture;

    u32 ubo_0;
    u32 ubo_1;
    Rig rig;
    Rig_Instance rig_instance;
}
Game;

// NOTE: globals implemented in win32_main.c (avoids linker errors)
extern Platform* platform;
extern Game* game;

// NOTE: Allocators ONLY for external libs like stbi_image.h to use.
#define LIB_ALLOCATION_MALLOC(size) arena_push_size(&game->frame_arena, size)
#define LIB_ALLOCATION_REALLOC(ptr, new_size) LIB_ALLOCATION_MALLOC(new_size)
// NOTE: Define free to nothing

void game_update_and_render(Platform* platform_);

#endif  // SK_H
