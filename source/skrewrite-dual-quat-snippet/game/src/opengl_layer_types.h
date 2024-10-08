#ifndef SK_OPENGL_LAYER_TYPES_H
#define SK_OPENGL_LAYER_TYPES_H

typedef struct Shader
{
    u32 id;
}
Shader;

typedef struct Texture2D
{
    u32 id;             // NOTE: OpenGL texture id
    s32 width, height;
    s32 color_channel_count;
}
Texture2D;

typedef enum
{
    ISLAND_DAY_CLEAR = 0, // 0, // Slightly warm light
    ISLAND_DAY_RAIN,      // 1, // Lightish gray (see windwaker)
    ISLAND_NIGHT_CLEAR,   // 2, // Moonlight colour
    ISLAND_NIGHT_RAIN,    // 3, // Blue gray colour
    INDOOR_LIGHT,         // 4, // Warm light
    INDOOR_DARK,          // 5, // dark gray

    PALETTE_STYLES_COUNT
} Palette_Styles;  // index into highlights & shadows arrays

typedef struct Material  // Super Struct
{
    // Specifier
    u32 map_count;
    b32 cel_shaded;

    // Components
    Vector4 tint;
#define MATERIAL_MAX_MAPS 3;
    Texture2D maps[MATERIAL_MAX_MAPS];

    // Pallete
    
    Vector3 highlights[PALLET_STYLES_COUNT];  // interpolate based on parameters
    Vector3 shadows[PALLET_STYLES_COUNT];
    /*
    E.g. weather controller should have rain intensity parameter of 0.0 to 1.0
    At rain=1.0, the palette is completely rain, (at time of day)
    At rain=0.5, the palette is interpolation of clear and rain (not necessarily linear, test different interpolations)
    */
}
Material;

typedef struct Vertex
{
    Vector3 p;  // position
    Vector2 t;  // texcoords
    Vector3 n;  // normal
}
Vertex;

typedef struct Mesh
{
    u32 vertex_count;
    u32 index_count;

    Vertex* vertices;
    u32* indices;

    // GPU:
    u32 vao, vbo, ebo;
    Texture2D texture;
}
Mesh;



#endif  // SK_OPENGL_LAYER_TYPES_H
