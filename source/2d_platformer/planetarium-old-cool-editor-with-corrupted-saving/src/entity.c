#include "entity.h"
#include "room.h"
#include "assets.h"
#include <cJSON.h>

// PLAYER
static void
entity_init_as_player(Entity* e)
{
    e->has_sprite = 1;
    e->has_mover  = 1;
    e->has_player = 1;

    // Sprite Component
    e->sprite.sprite_state.current_animation = 0;
    e->sprite.sprite_state.current_frame = 0;
    e->sprite.sprite_state.current_animation_time = 0.0f;

    // Mover Component
    e->mover.velocity = (Vector2){ 0.0f, 0.0f };
    e->mover.position_remainder = (Vector2){ 0.0f, 0.0f };
    e->mover.collision.type = COLLISION_TYPE_BOX;
    e->mover.collision.box = assets.sprite_player.collision_box;
    e->mover.on_ground = 0;
    e->mover.on_ceiling = 0;
    e->mover.on_wall_left = 0;
    e->mover.on_wall_right = 0;

    // Player Component
    e->player.is_facing_right = 1;
}

void
create_entity(Room* parent_room, Vector2 position, int type, Entity* out_e)
{
    ++parent_room->entity_count;
    out_e->position = position;
    out_e->parent_room = parent_room;
    out_e->type = type;
    switch (type)
    {
        case ENTITY_PLAYER:
            entity_init_as_player(out_e);
            break;
    }
}

void
update_entity(Entity* e, float dt)
{
    if (e->has_player)  component_player_update(e, dt);
    if (e->has_mover)   component_mover_update(e, dt);
    if (e->has_sprite)  component_sprite_update(e, dt);
}

void
draw_entity(Entity* e)
{
    if (e->has_player)  component_player_render(e);
    if (e->has_sprite)  component_sprite_render(e);
    if (e->has_mover)   component_mover_render(e);
}

int
entity_type_name_to_id(char* type_name)
{
    int type = -1;
    if (TextIsEqual(type_name, "player"))
    {
        // The player should occupy the first entity slot:
        // assert(i == 0);       // <<<<<<<<<<<<<<
        type = ENTITY_PLAYER;
    }
    // else if (.... "monster_bat")  etc...
    else
    {
        printf("Entity has no valid type: \"%s\"\n", type_name);
        assert(0);
    }

    return type;
}

char*
entity_type_id_to_name(int type_id)
{
    if (type_id == ENTITY_PLAYER)
    {
        return ENTITY_PLAYER_NAME;
    }
    // else if (...) etc...
    else
    {
        printf("Entity is not a valid type: %d\n", type_id);
        assert(0);
        return "entity_type_id_to_name error";
    }
}

// NOT NEEDED YET:
// cJSON*
// entity_to_json(Entity* e)
// {
//     cJSON* entity_json = cJSON_CreateObject();
//     if (entity_json == NULL)
//     {
//         printf("entity_to_json failed, exiting\n");
//     }

//     cJSON* spawn_point = cJSON_AddObjectToObject(entity_json, "spawn_point");
//     cJSON_AddNumberToObject(spawn_point, "x", (double)e->position.x);
//     cJSON_AddNumberToObject(spawn_point, "y", (double)e->position.y);

//     cJSON_AddStringToObject(entity_json, "type", entity_type_id_to_name(e->type));

//     return entity_json;
// }
