#ifndef PLANETARIUM_LEVEL_H
#define PLANETARIUM_LEVEL_H

#include <raylib.h>
#include "tilemap.h"
#include "entity.h"
#include "sprite.h"

#define ROOM_MAX_ENTITIES 128

typedef struct Room
{
    Vector2 world_position;
    Tilemap tilemap;

    // Camera:
    Entity* camera_target;
    Camera2D camera_viewport;

    ////////////

    int entity_count;
    union
    {
        Entity entities[ROOM_MAX_ENTITIES];
        struct
        {
            Entity player;
        };
    };
}
Room;

void load_room(const char* room_json_path, Room* out_room);
void reload_room(Room* room);
void update_room(Room* room, float dt);
void draw_room(Room* room);

void update_camera(Room* room);

#endif  // PLANETARIUM_LEVEL_H
