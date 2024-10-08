#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>


#include "game.h"
// #include "room.h"

void
draw_game_buffer_to_window(Game* game)
{
    float scale = min((float)GetScreenWidth()  / (float)game->width,
                        (float)GetScreenHeight() / (float)game->height);
    
    DrawTexturePro(game->buffer.texture, (Rectangle){ 0, game->height, game->width, -game->height }, (Rectangle){ 0.5f * (GetScreenWidth()  - game->width  * scale), 0.5f * (GetScreenHeight() - game->height * scale), game->width * scale, game->height * scale }, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
}

Vector2
world_to_screen_position(Game* game, Vector2 world_pos)
{
    float scale = min((float)GetScreenWidth()  / (float)game->width,
                        (float)GetScreenHeight() / (float)game->height);

    float x = world_pos.x * scale;
    x += 0.5f * (GetScreenWidth()  - game->width  * scale);

    float y = world_pos.y * scale;
    y += 0.5f * (GetScreenHeight() - game->height * scale);

    return (Vector2){ x, y };
}

Vector2
screen_to_buffer_position(Game* game, Vector2 screen_pos)
{
    float scale = min((float)GetScreenWidth()  / (float)game->width,
                    (float)GetScreenHeight() / (float)game->height);

    float x = screen_pos.x - 0.5f * (GetScreenWidth()  - game->width  * scale);
    x /= scale;

    float y = screen_pos.y - 0.5f * (GetScreenHeight() - game->height * scale);
    y /= scale;

    return (Vector2){ x, y };
}

Vector2
screen_to_world_position(Game* game, Room* current_room, Vector2 screen_pos)
{
    Vector2 pos = screen_to_buffer_position(game, screen_pos);

    pos.x += current_room->camera_viewport.target.x - current_room->camera_viewport.offset.x;
    pos.y += current_room->camera_viewport.target.y - current_room->camera_viewport.offset.y;

    return pos;
}



// Editor:

void
editor_camera_update(Game* game)
{
    if (IsKeyDown(KEY_A))
    {
        game->editor_room.camera_viewport.target.x -= 5.0f;
    }
    if (IsKeyDown(KEY_D))
    {
        game->editor_room.camera_viewport.target.x += 5.0f;
    }
    if (IsKeyDown(KEY_W))
    {
        game->editor_room.camera_viewport.target.y -= 5.0f;
    }
    if (IsKeyDown(KEY_S))
    {
        game->editor_room.camera_viewport.target.y += 5.0f;
    }
}
