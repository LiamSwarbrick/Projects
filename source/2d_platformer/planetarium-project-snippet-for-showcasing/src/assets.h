#ifndef PLANETARIUM_ASSETS_H
#define PLANETARIUM_ASSETS_H

#include <stdio.h>
#include <assert.h>
#include "sprite.h"
#include "tilemap.h"

typedef struct Assets
{
    Sprite sprite_player;

#define ASSETS_TILESET_COUNT 1
    union 
    {
        Tileset tilesets[ASSETS_TILESET_COUNT];
        Tileset tileset_underground;
    };
}
Assets;

extern Assets assets;

static int
load_assets()
{
    int failed_count = 0;
    failed_count += !load_sprite("assets/quote.aseprite", &assets.sprite_player);
    failed_count += !load_tileset(cJSON_Parse(LoadFileText("assets/tileset_underground.json")), &assets.tileset_underground);

    if (failed_count > 0)
    {
        printf("\nWarning: %d assets failed to load.\n\n", failed_count);
        return 0;
    }

    return 1;
}

static Tileset*
find_tileset_asset(char* name)
{
    for (int i = 0; i < ASSETS_TILESET_COUNT; ++i)
    {
        if (TextIsEqual(name, assets.tilesets[i].name))
        {
            return &assets.tilesets[i];
        }
        printf("\n%s\n%s\n", name, assets.tilesets[i].name);
    }

    printf("find_tileset_asset(\"%s\") could not find a matching tileset.\n", name);
    assert(0);
    return NULL;
}


#endif  // PLANETARIUM_ASSETS_H
