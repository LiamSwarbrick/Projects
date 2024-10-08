#include "sprite.h"

#include <stdlib.h>
#define CUTE_ASEPRITE_IMPLEMENTATION
#include <cute_aseprite.h>


int
load_sprite(const char* aseprite_path, Sprite* out)
{
    int status = 0;
    ase_t* aseprite = cute_aseprite_load_from_file(aseprite_path, NULL);
    if (aseprite == NULL)
    {
        printf("Failed to load aseprite file: %s\n", aseprite_path);
        goto end;
    }

    // Build a spritesheet out of each aseprite frame
    Texture2D spritesheet;
    int frame_count = aseprite->frame_count;
    float* frame_durations = malloc(frame_count * sizeof(float));
    {
        Image spritesheet_image = GenImageColor(aseprite->w * aseprite->frame_count, aseprite->h, BLANK);
        ImageFormat(&spritesheet_image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

        for (int i = 0; i < aseprite->frame_count; ++i)
        {
            Image sprite; 
            // NOTE: Do not call UnloadImage since the pixel data is freed by cute_aseprite_free()
            sprite.data = aseprite->frames[i].pixels;
            sprite.width = aseprite->w;
            sprite.height = aseprite->h;
            sprite.mipmaps = 1;
            sprite.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
            ImageDraw(&spritesheet_image, sprite, (Rectangle){ 0.0f, 0.0f, (float)sprite.width, (float)sprite.height }, (Rectangle){ (float)(sprite.width * i), 0.0f, sprite.width, sprite.height }, WHITE);

            frame_durations[i] = (float)aseprite->frames[i].duration_milliseconds / 1000.0f;
        }

        spritesheet = LoadTextureFromImage(spritesheet_image);
    }

    // Animation data
    int animation_count = aseprite->tag_count;
    Sprite_Animation* animations = calloc(animation_count, sizeof(Sprite_Animation));
    for (int i = 0; i < animation_count; ++i)
    {
        animations[i].name = malloc(TextLength(aseprite->tags[i].name));
        TextCopy(animations[i].name, aseprite->tags[i].name);
        animations[i].from = aseprite->tags[i].from_frame;
        animations[i].to = aseprite->tags[i].to_frame;
        animations[i].total_duration = 0.0f;
        for (int j = animations[i].from; j <= animations[i].to; ++j)
        {
            animations[i].total_duration += frame_durations[j];
        }

        // Animations with names prefixed with "L_" should loop
        if (TextLength(animations[i].name) >= 2)
        {
            if (animations[i].name[0] == 'L' && animations[i].name[1] == '_')
            {
                animations[i].loop = 1;
            }
        }
        else
        {
            animations[i].loop = 0;
        }
    }
    
    // Collision rectangle and position origin (pivot) from "collision" slice.
    // My pivots are relative to the top left of the sprite image, unlike in aseprite which are relative to the top left of the slice
    Vector2 pivot;
    Rectangle collision_rectangle = { 0 };
    if (aseprite->slice_count > 0)
    {
        for (int i = 0; i < aseprite->slice_count; ++i)
        {
            if (TextIsEqual(aseprite->slices[i].name, "collision"))
            {
                pivot.x = aseprite->slices[i].pivot_x + aseprite->slices[i].origin_x;
                pivot.y = aseprite->slices[i].pivot_y + aseprite->slices[i].origin_y;
                collision_rectangle.x = aseprite->slices[i].origin_x - pivot.x;
                collision_rectangle.y = aseprite->slices[i].origin_y - pivot.y;
                collision_rectangle.width = aseprite->slices[i].w;
                collision_rectangle.height = aseprite->slices[i].h;
            }

        }
    }

    out->spritesheet = spritesheet;
    out->width = aseprite->w;
    out->height = aseprite->h;
    out->frame_count = frame_count;
    out->frame_durations = frame_durations;
    out->animation_count = animation_count;
    out->animations = animations;
    out->collision_box = collision_rectangle;
    out->pivot = pivot;

    cute_aseprite_free(aseprite);
    status = 1;

end:
    return status;
}

void
unload_sprite(Sprite* sprite)
{
    if (sprite != NULL)
    {
        UnloadTexture(sprite->spritesheet);
        free(sprite->frame_durations);
        for (int i = 0; i < sprite->frame_count; ++i)
        {
            free(sprite->animations[i].name);
        }
        free(sprite->animations);
    }
}

Texture2D
load_texture_aseprite(const char* aseprite_path)
{
    ase_t* aseprite = cute_aseprite_load_from_file(aseprite_path, NULL);
    if (aseprite == NULL)
    {
        printf("Failed to load aseprite file: %s\n", aseprite_path);
        exit(1);
    }

    Texture2D texture;
    Image image;
    // NOTE: Do not call UnloadImage since the pixel data is freed by cute_aseprite_free()
    image.data = aseprite->frames[0].pixels;
    image.width = aseprite->w;
    image.height = aseprite->h;
    image.mipmaps = 1;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    
    texture = LoadTextureFromImage(image);

    cute_aseprite_free(aseprite);

    return texture;
}

int
begin_sprite_animation(Sprite* sprite, Sprite_State* state, char* name)
{
    for (int i = 0; i < sprite->animation_count; ++i)
    {
        if (TextIsEqual(sprite->animations[i].name, name))
        {
            state->current_animation = i;
            state->current_animation_time = 0.0f;
            state->current_frame = sprite->animations[i].from;
            return 1;
        }
    }

    printf("Requested animation \"%s\" does not exist. Exiting for debugging purposes.", name);
    return 0;
}

int
begin_or_continue_sprite_animation(Sprite* sprite, Sprite_State* state, char* name)
{
    // Return 0 if animation is continued
    if (!TextIsEqual(sprite->animations[state->current_animation].name, name))
    {
        begin_sprite_animation(sprite, state, name);
        return 1;
    }
    return 0;
}

void
update_sprite(Sprite* sprite, Sprite_State* state, float dt)
{
    state->current_animation_time += dt;
    if (sprite->animations[state->current_animation].loop || state->current_frame < sprite->animations[state->current_animation].to)
    {
        state->current_frame = sprite->animations[state->current_animation].from;
        float t = 0;
        while (t <= state->current_animation_time)
        {
            if (state->current_animation_time < sprite->animations[state->current_animation].total_duration - sprite->frame_durations[sprite->frame_count-1])
            {
                t += sprite->frame_durations[state->current_frame];
                ++state->current_frame;
            }
            else
            {
                state->current_animation_time -= sprite->animations[state->current_animation].total_duration;
                state->current_frame = sprite->animations[state->current_animation].from;

                break;
            }
        }
    }
    
}

void
draw_sprite(Sprite* sprite, Sprite_State state, Vector2 pos)
{
    Rectangle source = { (float)(state.current_frame * sprite->width), 0.0f, sprite->width, sprite->height };
    Rectangle dest = { pos.x, pos.y, sprite->width, sprite->height };
    DrawTexturePro(sprite->spritesheet, source, dest, sprite->pivot, 0.0f, WHITE);
}

void
draw_sprite_collision(Sprite* sprite, Vector2 pos)
{
    DrawRectangleLines(sprite->collision_box.x + pos.x, sprite->collision_box.y + pos.y, sprite->collision_box.width, sprite->collision_box.height, BLUE);
    DrawPixel(pos.x, pos.y, GREEN);
}