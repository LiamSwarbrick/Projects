#include "room.h"
#include <raylib.h>
#include "sprite.h"
#include <assert.h>
#include <cJSON.h>
#include <stdlib.h>

// Ideas:
// save_room()
// create_room_backup()

void
load_room(const char* room_json_path, Room* out_room)
{

    cJSON* room_json = cJSON_Parse(LoadFileText(room_json_path));
    cJSON* world_position_json = cJSON_GetObjectItemCaseSensitive(room_json, "world_position");
    cJSON* tilemap_json = cJSON_GetObjectItemCaseSensitive(room_json, "tilemap");
    cJSON* entities_json_array = cJSON_GetObjectItemCaseSensitive(room_json, "entities");

    // Load "world_position": { x: , y: }
    out_room->world_position.x = (float)cJSON_GetObjectItemCaseSensitive(world_position_json, "x")->valuedouble;
    out_room->world_position.y = (float)cJSON_GetObjectItemCaseSensitive(world_position_json, "y")->valuedouble;

    // Load "tilemap": { ... }
    if (!load_tilemap(tilemap_json, &out_room->tilemap))
    {
        printf("Failed to load tilemap\n");
        assert(0);
    }

    // Load "entities": [ ... ]
    int entity_count = cJSON_GetArraySize(entities_json_array);
    out_room->entity_count = 0;  // This is automatically incremented by create_entity()
    assert(out_room->entity_count <= ROOM_MAX_ENTITIES);
    
    for (int i = 0; i < entity_count; ++i)
    {
        cJSON* ith_entity_json = cJSON_GetArrayItem(entities_json_array, i);

        cJSON* spawn_point_json = cJSON_GetObjectItemCaseSensitive(ith_entity_json, "spawn_point");
        cJSON* type_json = cJSON_GetObjectItemCaseSensitive(ith_entity_json, "type");

        Vector2 position;
        int type;

        // Spawn point in json is relative to world position
        // But entity position is in world space.
        position.x = out_room->world_position.x;
        position.y = out_room->world_position.y;
        position.x += (float)cJSON_GetObjectItemCaseSensitive(spawn_point_json, "x")->valuedouble;
        position.y += (float)cJSON_GetObjectItemCaseSensitive(spawn_point_json, "y")->valuedouble;

        type = entity_type_name_to_id("player");

        create_entity(out_room, position, type, &out_room->entities[i]);
    }

    // Init Camera:
    out_room->camera_target = &out_room->player;
    update_camera(out_room);
}

void
overwrite_saved_room_with_new_rooms_tiles(Room* room, const char* room_json_path)
{
    cJSON* room_json = cJSON_CreateObject();
    if (room_json == NULL)
    {
        printf("Failed to save tiles for \"%s\".\n", room_json_path);
        goto end__save_room;
    }

    cJSON* world_position = cJSON_AddObjectToObject(room_json, "world_position");
    cJSON_AddNumberToObject(world_position, "x", (double)room->world_position.x);
    cJSON_AddNumberToObject(world_position, "y", (double)room->world_position.y);

    cJSON_AddItemToObject(room_json, "tilemap", tilemap_to_json(&room->tilemap));

    // Fetch original entities from room_json_path
    cJSON* original_room_json = cJSON_Parse(LoadFileText(room_json_path));
    cJSON* original_entities_json_array = cJSON_GetObjectItemCaseSensitive(room_json, "entities");

    // Spawn points remain unchanged:
    cJSON_AddItemToObject(room_json, "entities", original_entities_json_array);

    // Save new json to same path:
    char* string = cJSON_Print(room_json);
    SaveFileText(room_json_path, string);
    free(string);

end__save_room:
    cJSON_Delete(room_json);
}

void
update_room(Room* room, float dt)
{
    for (int i = 0; i < room->entity_count; ++i)
    {
        Entity* e = &room->entities[i];

        update_entity(e, dt);
    }

    update_camera(room);
}

void
draw_room(Room* room)
{
    BeginMode2D(room->camera_viewport);
    
        draw_tilemap(&room->tilemap, room->world_position);

        for (int i = 0; i < room->entity_count; ++i)
        {
            Entity* e = &room->entities[i];

            draw_entity(e);
        }
    
    EndMode2D();
}

