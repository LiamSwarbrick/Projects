#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include "game.h"
#include "level.h"

Game game = { .width       = 320,//256,  // 256 for recording
              .height      = 256,//224,
              .tile_size   =  16,
              .columns     =  320/16,
              .rows        =  256/16 };

Assets assets = { 0 };

int
main(int argc, char** argv)
{
    // Create window:
    if (argc != 1)
    {
        if (TextIsEqual(argv[1], "fullscreen"))
        {
            SetConfigFlags(FLAG_VSYNC_HINT | FLAG_FULLSCREEN_MODE);
            InitWindow(0, 0, "Planetarium");
        }
    }
    else
    {
        SetConfigFlags(FLAG_VSYNC_HINT);
        // InitWindow(640, 512, "Planetarium");  // For recording
        InitWindow(1600, 1280, "Planetarium");
    }
    

    // Init game:
    game.buffer = LoadRenderTexture(game.width, game.height);
    SetTextureFilter(game.buffer.texture, TEXTURE_FILTER_POINT);
    game.buffer_to_window_scale = min((float)GetScreenWidth()  / (float)game.width,
                                      (float)GetScreenHeight() / (float)game.height);

    game.render_collision_boxes = 0;
    game.debug_color_entity_collision = BLUE;
    game.debug_color_entity_position = GREEN;

    if (!load_assets())
    {
        printf("load_assets() failed, bye.\n");
        return 0;
    }

    // Editor init
    
    // Level init
    game.screen = SCREEN_PLATFORMER;
    load_room("assets/saved_room.json", &game.test_room);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_TAB) && game.screen != SCREEN_EDITOR)
        {
            game.screen = SCREEN_EDITOR;
            memcpy(&game.editor_room, &game.test_room, sizeof(Room));
        }
        
        if (IsKeyPressed(KEY_B))
            game.render_collision_boxes = !game.render_collision_boxes;

        // Gameplay:
        if (game.screen == SCREEN_PLATFORMER)
        {
            update_room(&game.test_room, dt);

            // Draw game
            BeginTextureMode(game.buffer);
                ClearBackground((Color){ 20, 18, 29, 255 });

                draw_room(&game.test_room);
        
            EndTextureMode();


            // Draw game buffer to window
            BeginDrawing();
                ClearBackground(BLACK);

                float scale = min((float)GetScreenWidth()  / (float)game.width,
                                (float)GetScreenHeight() / (float)game.height);
                game.buffer_to_window_scale = scale;
                DrawTexturePro(game.buffer.texture, (Rectangle){ 0, game.height, game.width, -game.height }, (Rectangle){ 0.5f * (GetScreenWidth()  - game.width  * scale), 0.5f * (GetScreenHeight() - game.height * scale), game.width * scale, game.height * scale }, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);

            EndDrawing();
        }
        // Editor:
        else if (game.screen == SCREEN_EDITOR)
        {
            // TODO: Switch to c dearimgui
            BeginDrawing();
                ClearBackground(BLACK);
                
                // Rectangle editor_buttons_box_bounds = { 0.0f, 0.0f, GetScreenHeight() / 5.0f, GetScreenHeight() };
                // int exit_button = GuiWindowBox(editor_buttons_box_bounds, "#198# PORTABLE WINDOW");
                // if (exit_button)
                // {
                //     load_room("assets/saved_room.json", &game.test_room);
                //     game.screen = SCREEN_PLATFORMER;
                // }

                
                
            EndDrawing();


        }

    }

    
    CloseWindow();

    return 0;
}
