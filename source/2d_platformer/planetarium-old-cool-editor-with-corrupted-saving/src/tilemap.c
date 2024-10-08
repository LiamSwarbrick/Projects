#include "tilemap.h"
#include "assets.h"

#include <raylib.h>
#include <cJSON.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int
load_tileset(const cJSON* tileset_object, Tileset* out_tileset)
{
    const cJSON* name          = NULL;
    const cJSON* texture_path  = NULL;
    const cJSON* tile_width    = NULL;
    const cJSON* tile_height   = NULL;

    if (tileset_object == NULL)
    {
        return 0;
    }
    
    name          = cJSON_GetObjectItemCaseSensitive(tileset_object, "name");
    texture_path  = cJSON_GetObjectItemCaseSensitive(tileset_object, "texture");
    tile_width    = cJSON_GetObjectItemCaseSensitive(tileset_object, "tile_width");
    tile_height   = cJSON_GetObjectItemCaseSensitive(tileset_object, "tile_height");

    if (name == NULL || texture_path == NULL || tile_width == NULL || tile_height == NULL)
    {
        return 0;
    }
    
    TextCopy(out_tileset->name, name->valuestring);
    // Read name into char[64]
    int bytes_copied = TextCopy(out_tileset->name, name->valuestring);
    assert(bytes_copied <= 64);

    out_tileset->tiles_texture  = load_texture_aseprite(texture_path->valuestring);
    out_tileset->tile_width     = tile_width->valueint;
    out_tileset->tile_height    = tile_height->valueint;

    return 1;
}

void
allocate_empty_tilemap_tiles(Tilemap* tilemap)
{
    // Allocate ground tiles in 2D array
    tilemap->ground     = malloc(tilemap->columns * sizeof(int*));
    tilemap->ground[0]  = calloc(tilemap->columns * tilemap->rows, sizeof(int));
    for (int x = 1; x < tilemap->columns; ++x)
    {
        tilemap->ground[x] = tilemap->ground[x-1] + tilemap->rows;
    }

    // Allocate background tiles in 2D array
    tilemap->bg         = malloc(tilemap->columns * sizeof(int*));
    tilemap->bg[0]      = calloc(tilemap->columns * tilemap->rows, sizeof(int));
    for (int x = 1; x < tilemap->columns; ++x)
    {
        tilemap->bg[x] = tilemap->bg[x-1] + tilemap->rows;
    }
}

void
free_tilemap_tiles(Tilemap* tilemap)
{
    free(tilemap->ground[0]);
    free(tilemap->ground);

    free(tilemap->bg[0]);
    free(tilemap->bg);
}

int
load_tilemap(const cJSON* tilemap_object, Tilemap* out_tilemap)
{
    const cJSON* tileset_name  = NULL;
    const cJSON* columns       = NULL;
    const cJSON* rows          = NULL;
    const cJSON* ground        = NULL;
    const cJSON* bg            = NULL;

    tileset_name  = cJSON_GetObjectItemCaseSensitive(tilemap_object, "tileset_name");
    columns       = cJSON_GetObjectItemCaseSensitive(tilemap_object, "columns");
    rows          = cJSON_GetObjectItemCaseSensitive(tilemap_object, "rows");
    ground        = cJSON_GetObjectItemCaseSensitive(tilemap_object, "ground");
    bg            = cJSON_GetObjectItemCaseSensitive(tilemap_object, "bg");

    // Read tileset_name into char[64]
    int bytes_copied = TextCopy(out_tilemap->tileset_name, tileset_name->valuestring);
    assert(bytes_copied <= 64);

    out_tilemap->columns    = columns->valueint;
    out_tilemap->rows       = rows->valueint;
    allocate_empty_tilemap_tiles(out_tilemap);

    // // Allocate ground tiles in 2D array
    // out_tilemap->ground     = malloc(out_tilemap->columns * sizeof(int*));
    // out_tilemap->ground[0]  = calloc(out_tilemap->columns * out_tilemap->rows, sizeof(int));
    // for (int x = 1; x < out_tilemap->columns; ++x)
    // {
    //     out_tilemap->ground[x] = out_tilemap->ground[x-1] + out_tilemap->rows;
    // }

    // // Allocate background tiles in 2D array
    // out_tilemap->bg         = malloc(out_tilemap->columns * sizeof(int*));
    // out_tilemap->bg[0]      = calloc(out_tilemap->columns * out_tilemap->rows, sizeof(int));
    // for (int x = 1; x < out_tilemap->columns; ++x)
    // {
    //     out_tilemap->bg[x] = out_tilemap->bg[x-1] + out_tilemap->rows;
    // }

    // Fill ground tiles with values from json
    for (int x = 0; x < out_tilemap->columns; ++x)
    {
        for (int y = 0; y < out_tilemap->rows; ++y)
        {
            out_tilemap->ground[x][y] = cJSON_GetArrayItem(ground, (x*out_tilemap->rows) + y)->valueint;
        }
    }

    // Fill bg tiles with values from json
    for (int x = 0; x < out_tilemap->columns; ++x)
    {
        for (int y = 0; y < out_tilemap->rows; ++y)
        {
            out_tilemap->bg[x][y] = cJSON_GetArrayItem(bg, (x*out_tilemap->rows) + y)->valueint;
        }
    }
}

cJSON*
tilemap_to_json(Tilemap* tilemap)
{
    cJSON* tilemap_json = cJSON_CreateObject();
    if (tilemap_json == NULL)
    {
        printf("tilemap_to_json failed, exiting\n");
        exit(-1);
    }

    cJSON* tileset_name = cJSON_AddStringToObject(tilemap_json, "tileset_name",    tilemap->tileset_name);
    cJSON* rows         = cJSON_AddNumberToObject(tilemap_json, "rows",    (double)tilemap->rows);
    cJSON* columns      = cJSON_AddNumberToObject(tilemap_json, "columns", (double)tilemap->columns);
    cJSON* ground       = cJSON_AddArrayToObject(tilemap_json, "ground");
    cJSON* bg           = cJSON_AddArrayToObject(tilemap_json, "bg");
    for (int y = 0; y < tilemap->columns; ++y)
    {
        for (int x = 0; x < tilemap->rows; ++x)
        {
            cJSON* ground_tile_id = cJSON_CreateNumber((double)tilemap->ground[x][y]);
            cJSON* bg_tile_id     = cJSON_CreateNumber((double)tilemap->bg[x][y]);
            cJSON_AddItemToArray(ground, ground_tile_id);
            cJSON_AddItemToArray(bg, bg_tile_id);
        }
    }

    return tilemap_json;
}

void
draw_tilemap(Tilemap* tilemap, Vector2 world_position)
{
    Tileset* tileset = find_tileset_asset(tilemap->tileset_name);

    // No offscreen culling
    for (int x = 0; x < tilemap->columns; ++x)
    {
        for (int y = 0; y < tilemap->rows; ++y)
        {
            // Background tile
            {
                int tileid_x = tilemap->bg[x][y] % (tileset->tiles_texture.width / tileset->tile_width);
                int tileid_y = tilemap->bg[x][y] / (tileset->tiles_texture.width / tileset->tile_width);
                Rectangle source = { tileid_x * tileset->tile_width, tileid_y * tileset->tile_height, tileset->tile_width, tileset->tile_height };
                Rectangle dest = { world_position.x + (x * tileset->tile_width), world_position.y + (y * tileset->tile_height), tileset->tile_width, tileset->tile_height };
                DrawTexturePro(tileset->tiles_texture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
            }

            // Ground tile (foreground)
            {
                int tileid_x = tilemap->ground[x][y] % (tileset->tiles_texture.width / tileset->tile_width);
                int tileid_y = tilemap->ground[x][y] / (tileset->tiles_texture.width / tileset->tile_width);
                Rectangle source = { tileid_x * tileset->tile_width, tileid_y * tileset->tile_height, tileset->tile_width, tileset->tile_height };
                Rectangle dest = { world_position.x + (x * tileset->tile_width), world_position.y + (y * tileset->tile_height), tileset->tile_width, tileset->tile_height };
                DrawTexturePro(tileset->tiles_texture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }
    }
}
