#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include <math.h>


#include "game.h"
// #include "room.h"

Game game = { .width       = 320,//256,
              .height      = 256,//224,
              .tile_size   =  16,
              .columns     =  320/16,
              .rows        =  256/16 };

Assets assets = { 0 };

int
main(int argc, char** argv)
{
    // Create window:
    SetTargetFPS(60);

    if (argc != 1)
    {
        if (TextIsEqual(argv[1], "fullscreen"))
        {
            SetConfigFlags(FLAG_FULLSCREEN_MODE);
            // SetConfigFlags(FLAG_VSYNC_HINT | FLAG_FULLSCREEN_MODE);
            InitWindow(0, 0, "Planetarium");
        }
    }
    else
    {
        SetConfigFlags(0);
        // SetConfigFlags(FLAG_VSYNC_HINT);
        InitWindow(910, 512, "Planetarium");
    }
    

    // Init game:
    game.buffer = LoadRenderTexture(game.width, game.height);
    SetTextureFilter(game.buffer.texture, TEXTURE_FILTER_POINT);

    game.render_collision_boxes = 0;
    game.debug_color_entity_collision = BLUE;
    game.debug_color_entity_position = GREEN;

    if (!load_assets())
    {
        printf("load_assets() failed, bye.\n");
        return 0;
    }

    // Set screen
    game.screen = SCREEN_PLATFORMER;

    
    // Level init
    // game.playing_room_filename
    TextCopy(game.playing_room_filename, "assets/saved_room.json");
    load_room(game.playing_room_filename, &game.playing_room);

    // Editor init
    game.selected_tile = 1;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();


        // Switch between platformer and editor screen
        if (IsKeyPressed(KEY_TAB))
        {
            if (game.screen == SCREEN_PLATFORMER)
            {
                game.screen = SCREEN_EDITOR;

                memcpy(&game.editor_room, &game.playing_room, sizeof(Room));
            }
            else if (game.screen == SCREEN_EDITOR)
            {
                game.screen = SCREEN_PLATFORMER;
            }
        }


// Gameplay:
        if (game.screen == SCREEN_PLATFORMER)
        {
            update_room(&game.playing_room, dt);

            // Draw to game buffer
            BeginTextureMode(game.buffer);
                ClearBackground(DARKGRAY);

                draw_room(&game.playing_room);
        
            EndTextureMode();


            // Draw to window
            BeginDrawing();
                ClearBackground(BLACK);

                draw_game_buffer_to_window(&game);

            EndDrawing();
        }
// Editor:
        else if (game.screen == SCREEN_EDITOR)
        {
            
            editor_camera_update(&game);

            Vector2 mouse_world = screen_to_world_position(&game, &game.editor_room, GetMousePosition());
            Vector2 mouse_room = { mouse_world.x - game.editor_room.world_position.x, mouse_world.y - game.editor_room.world_position.y };

            int tile_world_cursor_pos_x = game.tile_size * (int)floorf(mouse_world.x / (float)game.tile_size);
            int tile_world_cursor_pos_y = game.tile_size * (int)floorf(mouse_world.y / (float)game.tile_size);

            int tile_room_id_x  = (int)floorf(mouse_room.x / (float)game.tile_size);
            int tile_room_id_y  = (int)floorf(mouse_room.y / (float)game.tile_size);

            // Check if mouse is within the letterboxed framebuffer
            Vector2 mouse_frame_buffer_pos = screen_to_buffer_position(&game, GetMousePosition());
            if (mouse_frame_buffer_pos.x >= 0 && mouse_frame_buffer_pos.x < game.width &&
                mouse_frame_buffer_pos.y >= 0 && mouse_frame_buffer_pos.y < game.height)
            {
                // Check if mouse is within the tilemaps boundaries:
                if (tile_room_id_x >= 0 && tile_room_id_x < game.editor_room.tilemap.columns &&
                    tile_room_id_y >= 0 && tile_room_id_y < game.editor_room.tilemap.rows)
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        game.editor_room.tilemap.ground[tile_room_id_x][tile_room_id_y] = game.selected_tile;
                    }
                    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                    {
                        game.editor_room.tilemap.ground[tile_room_id_x][tile_room_id_y] = 0;
                    }
                }
            }
            
            // Save tiles with space button and create backup
            if (IsKeyPressed(KEY_SPACE))
            {
            // Create file backup for the original:
                char* playing_room_original_file_text = LoadFileText(game.playing_room_filename);

                // e.g. "saved_room.json" to "saved_room-backup.json"
                char playing_room_backup_filename[ROOM_FILENAME_MAX_LENGTH + sizeof("-backup")] = { 0 };
                TextCopy(playing_room_backup_filename, game.playing_room_filename);

                int backup_position = TextFindIndex(playing_room_backup_filename, ".json");
                char* backup_filename_buffer = TextInsert(game.playing_room_filename, "-backup", backup_position);
                TextCopy(playing_room_backup_filename, backup_filename_buffer);
                free(backup_filename_buffer);

                SaveFileText(playing_room_backup_filename, playing_room_original_file_text);
                free(playing_room_original_file_text);

            // Overwrite file with new tiles:
                // Copy editor tiles to playing tiles
                free_tilemap_tiles(&game.playing_room.tilemap);
                memcpy(&game.playing_room.tilemap, &game.editor_room.tilemap, sizeof(Tilemap));

                // Save the new playing_room struct
                overwrite_saved_room_with_new_rooms_tiles(&game.editor_room, game.playing_room_filename);
                
                printf("File saved and backup created!\n");
            }

            // Draw to game buffer
            BeginTextureMode(game.buffer);
                ClearBackground(DARKBLUE);

                draw_room(&game.editor_room);

                BeginMode2D(game.editor_room.camera_viewport);
                    // Draw room borders:
                    DrawRectangleLines(game.editor_room.world_position.x, game.editor_room.world_position.y, game.editor_room.tilemap.columns * game.tile_size, game.editor_room.tilemap.rows * game.tile_size, WHITE);

                    // Tile cursor for mouse:
                    DrawRectangleLines(tile_world_cursor_pos_x, tile_world_cursor_pos_y, game.tile_size, game.tile_size,  PURPLE);
                EndMode2D();


            EndTextureMode();

            // Draw to window
            BeginDrawing();

                ClearBackground(BLACK);
                
                draw_game_buffer_to_window(&game);
                
            EndDrawing();


        }



    }

    
    CloseWindow();

    return 0;
}
