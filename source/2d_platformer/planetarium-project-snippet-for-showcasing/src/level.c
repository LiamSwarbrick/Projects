#include "level.h"
#include <raylib.h>
#include "sprite.h"
#include <assert.h>
#include <cJSON.h>

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

        if (TextIsEqual(type_json->valuestring, "player"))
        {
            // The player should occupy the first entity slot:
            // assert(i == 0);       // <<<<<<<<<<<<<<
            type = ENTITY_PLAYER;
        }
        // else if (.... "monster_bat")  etc...
        else
        {
            printf("Entity has no valid type: \"%s\"\n", type_json->valuestring);
            assert(0);
        }


        create_entity(out_room, position, type, &out_room->entities[i]);
    }

    // Init Camera:
    out_room->camera_target = &out_room->player;
    update_camera(out_room);
}

// void
// reload_room(Room* room)
// {
//     // save_room(room, room_json_path + "-backup");
//     load_room(room->room_json_path, room);
// }

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

