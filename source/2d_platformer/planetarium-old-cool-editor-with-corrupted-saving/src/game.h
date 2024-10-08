#ifndef PLANETARIUM_GAME_H
#define PLANETARIUM_GAME_H

#include <raylib.h>

#include "assets.h"
#include "room.h"

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

    int render_collision_boxes;
    Color debug_color_entity_collision;
    Color debug_color_entity_position;
    
// Screen management:
#define SCREEN_EDITOR 0
#define SCREEN_PLATFORMER 1
    int screen;

// Platformer:
#define ROOM_FILENAME_MAX_LENGTH 128
    char playing_room_filename[ROOM_FILENAME_MAX_LENGTH];
    Room playing_room;

// Editor:
    Room editor_room;
// #define EDIT_MODE_SELECT_ENTITIES 0
// #define EDIT_MODE_PLACE_ENTITIES  1
// #define EDIT_MODE_MOVE_ENTITIES   2
// #define EDIT_MODE_PLACE_TILES     3
//     int edit_mode;
    int selected_tile;
    
}
Game;

extern Game game;

void draw_game_buffer_to_window(Game* game);
Vector2 world_to_screen_position(Game* game, Vector2 world_pos);
Vector2 screen_to_buffer_position(Game* game, Vector2 screen_pos);
Vector2 screen_to_world_position(Game* game, Room* current_room, Vector2 screen_pos);

// Editor
void editor_camera_update(Game* game);

#endif  // PLANETARIUM_GAME_H
