#include <raylib.h>
#include <raymath.h>
#include "room.h"
#include "game.h"
#include <microui.h>
#include <microui_atlas.h>

#include <stdlib.h>

mu_Context* ctx;

int
ui_text_width(mu_Font font, const char* text, int len)
{
    if (len == -1) { len = TextLength(text); }

    int res = 0;
    for (const char* p = text; (*p != NULL) && len-- > 0; ++p)
    {
        if ((*p & 0xc0) == 0x80) { continue; }
        int chr = mu_min((unsigned char)*p, 127);
        res += atlas[ATLAS_FONT + chr].w;
    }
    return res;
}

int
ui_text_height(mu_Font font)
{
    return 18;
}

void
editor_init()
{
    // Init microui
    ctx = malloc(sizeof(mu_Context));
    mu_init(ctx);
    ctx->text_width = ui_text_width;
    ctx->text_height = ui_text_height;
}

void
editor_microui_update()
{
    Vector2 mouse_movement = GetMouseDelta();
    float mouse_wheel_movement = GetMouseWheelMove();
    mu_input_mousemove(ctx, (int)mouse_movement.x, (int)mouse_movement.y);
    mu_input_scroll(ctx, 0, (int)mouse_wheel_movement * -30);
    // mu_input_text not implemented
    
    Vector2 mouse_position = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { mu_input_mousedown(ctx, (int)mouse_position.x, (int)mouse_position.y, MU_MOUSE_LEFT) };
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { mu_input_mouseup(ctx, (int)mouse_position.x, (int)mouse_position.y, MU_MOUSE_LEFT) };
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { mu_input_mousedown(ctx, (int)mouse_position.x, (int)mouse_position.y, MU_MOUSE_RIGHT) };
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) { mu_input_mouseup(ctx, (int)mouse_position.x, (int)mouse_position.y, MU_MOUSE_RIGHT) };
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) { mu_input_mousedown(ctx, (int)mouse_position.x, (int)mouse_position.y, MU_MOUSE_MIDDLE) };
    if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) { mu_input_mouseup(ctx, (int)mouse_position.x, (int)mouse_position.y, MU_MOUSE_MIDDLE) };

    // keydown not implemented
    // keyup not implemented

    // Process Frame
    mu_begin(ctx);
    

    // Render
    BeginDrawing();
        ClearBackground(RAYWHITE);

        mu_Command* cmd = NULL;
        while (mu_next_command(ctx, &cmd))
        {
            switch (cmd->type)
            {
                case MU_COMMAND_TEXT: 
            }
        }
}

void
editor_new_room(Vector2 world_position, int columns, int rows, Tileset* in_tileset, Room* out_room)
{
    // Set room metadata from args
    out_room->world_position = world_position;
    TextCopy(out_room->tilemap.tileset_name, in_tileset->name);
    out_room->tilemap.columns = columns;
    out_room->tilemap.rows = rows;

    allocate_empty_tilemap_tiles(&out_room->tilemap);

    // Init room with player entity
    create_entity(out_room, (Vector2){ 0.0f, 0.0f }, ENTITY_PLAYER, &out_room->player);
}
