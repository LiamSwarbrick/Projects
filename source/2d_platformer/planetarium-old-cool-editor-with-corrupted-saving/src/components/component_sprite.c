#include <raylib.h>
#include <assert.h>
#include "../game.h"
#include "../entity.h"
#include "../sprite.h"


Sprite*
get_sprite(Entity* e)
{
    Sprite* sprite = NULL;
    if (e->type == ENTITY_PLAYER)
    {
        sprite = &assets.sprite_player;
    }

    assert(sprite != NULL);

    return sprite;
}

void
component_sprite_update(Entity* e, float dt)
{
    update_sprite(get_sprite(e), &e->sprite.sprite_state, dt);
}

void
component_sprite_render(Entity* e)
{
    // printf("(%f, %f)", e->position.x, e->position.y);
    draw_sprite(get_sprite(e), e->sprite.sprite_state, e->position);
}
