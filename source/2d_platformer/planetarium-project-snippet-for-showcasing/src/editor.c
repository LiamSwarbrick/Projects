#include <raylib.h>
#include "level.h"
#include "game.h"

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
