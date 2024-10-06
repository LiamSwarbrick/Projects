#include <raylib.h>
#include <math.h>
#include "../game.h"
#include "../entity.h"

static int
AABB(Rectangle a, Rectangle b)
{
    int a_left   = a.x;
    int a_top    = a.y;
    int b_left   = b.x;
    int b_top    = b.y;

    int a_right  = a.x + a.width;
    int a_bottom = a.y + a.height;
    int b_right  = b.x + b.width;
    int b_bottom = b.y + b.height;

    return a_left < b_right && a_right > b_left &&
           a_top < b_bottom && a_bottom > b_top;

}

static Collision
mover_collision_worldspace(Entity* e)
{
    Collision c = e->mover.collision;

    if (c.type == COLLISION_TYPE_BOX)
    {
        c.box.x += e->position.x;
        c.box.y += e->position.y;
    }

    return c;
}

static int
mover_tilemap_collision_check(Entity* e)
{
    Rectangle collider = mover_collision_worldspace(e).box;

    int left_tile = (collider.x - e->parent_room->world_position.x) / game.tile_size;
    int right_tile = (collider.x + collider.width - e->parent_room->world_position.x) / game.tile_size;
    int top_tile = (collider.y - e->parent_room->world_position.y) / game.tile_size;
    int bottom_tile = (collider.y + collider.height - e->parent_room->world_position.y) / game.tile_size;
    
    // min and max functions so that the tilemap isn't indexed out of range if collider is out of room bounds
    int has_collided = 0;
    for (int i = max(left_tile, 0); i <= min(right_tile, e->parent_room->tilemap.columns); ++i)
    {
        for (int j = max(top_tile, 0); j <= min(bottom_tile, e->parent_room->tilemap.columns); ++j)
        {
            if (i >= e->parent_room->tilemap.columns || j >= e->parent_room->tilemap.rows)
            {
                continue;
            }
            
            if (e->parent_room->tilemap.ground[i][j] != 0)
            {
                has_collided = 1;

                break;
            }
        }
    }

    return has_collided;
}

static void
mover_move_x(Entity* e, int dist)
{
    Vector2 old_position = e->position;
    e->mover.on_wall_left = 0;
    e->mover.on_wall_right = 0;

    while (abs(dist) > 0)
    {
        if (dist > 0)
        {
            ++e->position.x;
            --dist;
        }
        else
        {
            --e->position.x;
            ++dist;
        }

        if (mover_tilemap_collision_check(e))
        {
            e->position.x = old_position.x;
            if (e->mover.velocity.x > 0)
            {
                e->mover.on_wall_right = 1;
            }
            else
            {
                e->mover.on_wall_left = 1;
            }

            e->mover.position_remainder.x = 0;
            e->mover.velocity.x = 0;

            break;
        }
    }
}

static void
mover_move_y(Entity* e, int dist)
{
    e->mover.on_ground = 0;
    e->mover.on_ceiling = 0;

    while (abs(dist) > 0)
    {
        Vector2 old_position = e->position;
        if (dist > 0)
        {
            ++e->position.y;
            --dist;
        }
        else
        {
            --e->position.y;
            ++dist;
        }

        if (mover_tilemap_collision_check(e))
        {
            e->position.y = old_position.y;
            if (e->mover.velocity.y > 0)
            {
                e->mover.on_ground = 1;
                e->mover.position_remainder.y = 0.99f;  // This makes sure that on_ground stays true, the others like on_ceiling we only care when the player is moving into them
            }
            else
            {
                e->mover.on_ceiling = 1;
            }
        
            e->mover.velocity.y = 0;

            break;
        }
    }
}

void
component_mover_update(Entity* e, float dt)
{    
    // Move x
    float distx_f = e->mover.position_remainder.x + (e->mover.velocity.x * dt);
    int distx = (int)distx_f;
    e->mover.position_remainder.x = distx_f - (float)distx;
    mover_move_x(e, distx);

    // Move y
    float disty_f = e->mover.position_remainder.y + (e->mover.velocity.y * dt);
    int disty = (int)disty_f;
    e->mover.position_remainder.y = disty_f - (float)disty;
    mover_move_y(e, disty);

    if (mover_tilemap_collision_check(e) && e->mover.velocity.x == 0 && e->mover.velocity.y == 0)  // Fix when Teleported inside wall
    {
        --e->position.y;
    }

}

void
component_mover_render(Entity* e)
{
    if (game.render_collision_boxes)
    {
        Collision c = mover_collision_worldspace(e);
        
        if (c.type == COLLISION_TYPE_BOX)
        {
            DrawRectangleLinesEx(c.box, 1.0f, game.debug_color_entity_collision);
            DrawPixel(e->position.x, e->position.y, game.debug_color_entity_position);
        }
    }
}
