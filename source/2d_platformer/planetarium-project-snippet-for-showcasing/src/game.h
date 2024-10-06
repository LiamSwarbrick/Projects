#ifndef PLANETARIUM_GAME_H
#define PLANETARIUM_GAME_H

#include <raylib.h>

#include "assets.h"
#include "level.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct Game
{
    const int width;
    const int height;
    const int tile_size;
    const int columns;
    const int rows;

    RenderTexture2D buffer;
    float buffer_to_window_scale;

    int render_collision_boxes;
    Color debug_color_entity_collision;
    Color debug_color_entity_position;
    

#define SCREEN_EDITOR 0
#define SCREEN_PLATFORMER 1
    int screen;

    Room test_room;

// Editor:
    Room editor_room;
#define EDIT_MODE_SELECT_ENTITIES 0
#define EDIT_MODE_PLACE_ENTITIES  1
#define EDIT_MODE_MOVE_ENTITIES   2
#define EDIT_MODE_PLACE_TILES     3
    int edit_mode;
    
}
Game;

extern Game game;

#endif  // PLANETARIUM_GAME_H
