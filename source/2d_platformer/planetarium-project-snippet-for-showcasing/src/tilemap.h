#ifndef PLANETARIUM_TILEMAP_H
#define PLANETARIUM_TILEMAP_H

#include <raylib.h>
#include <cJSON.h>

#define TILESET_NAME_MAX_CHARS 64

typedef struct Tileset
{
    char name[TILESET_NAME_MAX_CHARS];
    Texture2D tiles_texture;
    int tile_width;
    int tile_height;
}
Tileset;

typedef struct Tilemap
{
    char tileset_name[TILESET_NAME_MAX_CHARS];
    int columns;
    int rows;
    int** ground;  // non-zero tiles are solid
    int** bg;
}
Tilemap;

int load_tileset(const cJSON* tileset_object, Tileset* out_tileset);
void allocate_empty_tilemap_tiles(Tilemap* tilemap);
int load_tilemap(const cJSON* tilemap_object, Tilemap* out_tilemap);
void draw_tilemap(Tilemap* tilemap, Vector2 world_position);

#endif  // PLANETARIUM_TILEMAP_H
