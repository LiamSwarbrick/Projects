#include <raylib.h>
#include <math.h>
#include <raymath.h>
#include "../game.h"
#include "../entity.h"
#include "../sprite.h"

// Pixels per second

// Horizontals:
const float accel = 300.0f;
const float max_speed = 130.0f;
const float friction = 900.0f;
const float air_resistance = 500.0f;

// Verticals:
const float gravity = 1100.0f;
const float jumping_gravity = gravity * 0.4f;
const float terminal_falling_velocity = 2700.0f;
const float max_jump_height = 70.0f;

// Input timers
//     Coyate Time:
const float max_coyate_time = 0.1f;
float time_since_last_grounded = 0.0f;
//     Jump button just before landing:
const float max_prejump_time = 0.2f;
float time_since_prejump = 0.0f;

void
component_player_update(Entity* e, float dt)
{
    // Buffered inputs:
    if (e->mover.on_ground)
    {
        time_since_last_grounded = 0.0f;
    }
    else
    {
        time_since_last_grounded += dt;
    }

    int jump_key = IsKeyDown(KEY_SPACE);
    int jump_key_pressed = IsKeyPressed(KEY_SPACE);

    if (jump_key_pressed && !e->mover.on_ground)
    {
        time_since_prejump = 0.0f;
    }
    else if (!e->mover.on_ground)
    {
        time_since_prejump += dt;
    }

    if (e->mover.on_ground && time_since_prejump <= max_prejump_time)
    {
        jump_key_pressed = 1;
    }

    int should_jump = jump_key_pressed && !e->player.is_jumping && (e->mover.on_ground || time_since_last_grounded <= max_coyate_time);

    // Update animation
    int input_x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    if (e->mover.on_ground)
    {
        if (input_x == 1)
        {
            begin_or_continue_sprite_animation(&assets.sprite_player, &e->sprite.sprite_state, "L_run-right");
        }
        else if (input_x == -1)
        {
            begin_or_continue_sprite_animation(&assets.sprite_player, &e->sprite.sprite_state, "L_run-left");
        }
        else
        {
            if (e->player.is_facing_right)
            {
                begin_or_continue_sprite_animation(&assets.sprite_player, &e->sprite.sprite_state, "idle-right");
            }
            else
            {
                begin_or_continue_sprite_animation(&assets.sprite_player, &e->sprite.sprite_state, "idle-left");
            }
        }
    }
    else
    {
        if (e->player.is_facing_right)
            {
                begin_or_continue_sprite_animation(&assets.sprite_player, &e->sprite.sprite_state, "jump-right");
            }
            else
            {
                begin_or_continue_sprite_animation(&assets.sprite_player, &e->sprite.sprite_state, "jump-left");
            }
    }

    // Horizontal movement
    {
        float current_accel = accel;
        if (e->mover.velocity.x != 0)
        {
            // If trying to move in the opposite direction of current velocity, apply friction/air_resistance value instead of accel value.
            if ((input_x == 1 && e->mover.velocity.x > 0) || (input_x == -1 && e->mover.velocity.x < 0))
            {
                current_accel = accel;
                
            }
            else if (e->mover.on_ground)
            {
                current_accel = friction;
            }
            else
            {
                current_accel = air_resistance;
            }
        }
        e->mover.velocity.x += (float)input_x * current_accel * dt;

        // Noel Berry cool thing: When over max speed, lerp speed back to max
        if (fabsf(e->mover.velocity.x) > max_speed)
        {
            if (e->mover.velocity.x > 0)
            {
                e->mover.velocity.x = max(e->mover.velocity.x - (2000.0f * dt),  max_speed);
            }
            else if (e->mover.velocity.x < 0)
            {
                e->mover.velocity.x = min(e->mover.velocity.x + (2000.0f * dt), -max_speed);
            }

        }

        // Friction / Air resistance
        if (input_x == 0)
        {
            float resistance = e->mover.on_ground ? friction : air_resistance;
            
            if (e->mover.velocity.x > 0)
            {
                e->mover.velocity.x = max(e->mover.velocity.x - (resistance * dt), 0.0f);
            }
            else if (e->mover.velocity.x < 0)
            {
                e->mover.velocity.x = min(e->mover.velocity.x + (resistance * dt), 0.0f);
            }
        }


    }

    // Vertical movement
    {
        // Terminal falling velocity
        if (e->mover.velocity.y > terminal_falling_velocity)
        {
            e->mover.velocity.y = max(e->mover.velocity.y - (2000.0f * dt),  max_speed);
        }
        

        // Jumping:
        if ((!jump_key || e->mover.velocity.y >= 0) && e->player.is_jumping)
        {
            e->player.is_jumping = 0;
        }

        if (should_jump)
        {
            e->player.is_jumping = 1;

            // Calculate jump impulse from jump_height
            float jump_impulse_velocity = -sqrtf(2.0f * max_jump_height * jumping_gravity);

            e->mover.velocity.y = jump_impulse_velocity;
        }


        // Apply gravity
        if (!e->player.is_jumping)
        {
            e->mover.velocity.y += gravity * dt;
        }
        else
        {
            e->mover.velocity.y += jumping_gravity * dt;
        }
    }

    // Facing direction
    if (e->mover.on_ground)
    {
        if (input_x != 0)
        {
            e->player.is_facing_right = input_x == 1;
        }
    }
}

void
component_player_render(Entity* e)
{

}
