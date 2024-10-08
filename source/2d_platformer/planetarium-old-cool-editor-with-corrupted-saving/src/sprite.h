#ifndef PLANETARIUM_SPRITE_H
#define PLANETARIUM_SPRITE_H

#include <raylib.h>

typedef struct Sprite_Animation
{
    char* name;
    int from;
    int to;
    int loop;
    float total_duration;
}
Sprite_Animation;

typedef struct Sprite
{
    Texture2D spritesheet;
    int width;  // width of individual sprite, not the spritesheet
    int height;

    int frame_count;
    float* frame_durations;

    int animation_count;
    Sprite_Animation* animations;

    Rectangle collision_box;
    Vector2 pivot;  // relative to top-left corner 
}
Sprite;

typedef struct Sprite_State
{
    int current_animation;
    int current_frame;
    float current_animation_time;
}
Sprite_State;

int load_sprite(const char* aseprite_path, Sprite* out);
void unload_sprite(Sprite* sprite);
Texture2D load_texture_aseprite(const char* aseprite_path);
int begin_sprite_animation(Sprite* sprite, Sprite_State* state, char* name);
int begin_or_continue_sprite_animation(Sprite* sprite, Sprite_State* state, char* name);
void update_sprite(Sprite* sprite, Sprite_State* state, float dt);
void draw_sprite(Sprite* sprite, Sprite_State state, Vector2 pos);
void draw_sprite_collision(Sprite* sprite, Vector2 pos);

#endif  // PLANETARIUM_SPRITE_H
