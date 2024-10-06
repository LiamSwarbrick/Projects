#ifndef PLANETARIUM_ENTITY_H
#define PLANETARIUM_ENTITY_H

#include <raylib.h>
#include <stdio.h>
#include "sprite.h"
#include "collision.h"
#include "tilemap.h"

enum Entity_Types
{
    ENTITY_PLAYER=0,
};

typedef struct Component_Sprite
{
    Sprite_State sprite_state;
}
Component_Sprite;


typedef struct Component_Mover
{
    Vector2 velocity;
    Vector2 position_remainder;  // Move by whole pixels but store subpixel value for next frame
    Collision collision;

    int on_ground;
    int on_ceiling;
    int on_wall_left;
    int on_wall_right;
}
Component_Mover;


typedef struct Component_Player
{
    int is_facing_right;

    int is_jumping;  // the key is still being held since leaving the ground
}
Component_Player;

// Forward declaration necessary since Entity contains a pointer to its parent room
typedef struct Room Room;

typedef struct Entity
{
    Vector2 position;
    int type;
    Room* parent_room;

    // Bools:
    unsigned char has_sprite;
    unsigned char has_mover;
    unsigned char has_player;

    // Components:
    Component_Sprite  sprite;
    Component_Mover   mover;
    Component_Player  player;
}
Entity;



Sprite* get_sprite(Entity* e);
void component_sprite_update(Entity* e, float dt);
void component_sprite_render(Entity* e);

void component_mover_update(Entity* e, float dt);
void component_mover_render(Entity* e);

void component_player_update(Entity* e, float dt);
void component_player_render(Entity* e);

void create_entity(Room* parent_room, Vector2 position, int type, Entity* out_e);
void update_entity(Entity* e, float dt);
void draw_entity(Entity* e);


#endif  // PLANETARIUM_ENTITY_H
