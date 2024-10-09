# 2D Platformer Engine in C with Raylib

[Source code](source/2d_platformer/planetarium-project-snippet-for-showcasing/src)

I made this 2D platformer game engine in C.
Its modularly programmed so that tooling is easy. The asset pipeline is seamlessly integrated with Aseprite (a popular pixel art animation software). The program is structured in a way that is easy to expand into a full game. For this project I decided to use Raylib instead of OpenGL so I could get straight to rendering quads to the screen.

![Tile editor](files/planetarium-room-editor-fun.gif)

| Character controller        |
|:----------------------------|
| I made sure to get the character controller feeling good, implementing ground friction, air resistance, coyote time and control over jump height.
I set these parameters to give tight feeling control of the player character. I've had plenty of experience releasing 2D games for game-jams, getting lots of feedback from players, and its always a good feeling player controller that people really notice!|

![Animation showcase](files/planetarium-animation-showcase-final.gif)

### Sprite system loading Aseprite files

<img src="files/planetarium-collision-box2.PNG" alt="Collision box from aseprite" width="60%">

Entities can have a sprite state component which handles all there sprite updates in a very easy way.

Directly loads the following data from Aseprite files:
- Animation names (Prefix name with 'L_' for my program to recognise it as a looping animation)
- How long each individual frame lasts
- Collision box setting through Aseprite's slices feature
- Setting pivot (origin) of sprite


```c
typedef struct Sprite
{
    Texture2D spritesheet;
    int width;  // width of individual sprite, not the spritesheet
    int height;

    int frame_count;
    float* frame_durations;

    int animation_count;
    Sprite_Animation* animations;

    Rectangle collision_box;
    Vector2 pivot;  // relative to top-left corner 
}
Sprite;
```

### Level creation and serialisation

I needed to make a simple editor for creating the tile maps.
You can see below the edited room is outlined. For a full game the levels/maps will be a collection of rooms (any width and height in tiles) that can be pieced together in the editor.
Each room will have a different camera controller, and a camera transition will occur when moving to another room.

![Tile editor](files/planetarium-room-editor.gif)

Serialisation uses JSON and I kept it simple and clean.

Here I've added lots of player entities for fun:

![Gameplay Example](files/planetarium-camera-and-entities.gif)

Here's the JSON file for this room of entities and tilemap.

```json
{
    "world_position": { "x": 0, "y": 0 },
	"tilemap": {
		"tileset_name": "tileset_underground",
		"rows":	14,
		"columns":	16,
		"ground":	[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
		"bg":	[2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
	},
	"entities": [
		{
			"spawn_point": { "x": 128, "y": 128 },
			"type": "player"
		},
		{
			"spawn_point": { "x": 100, "y": 64 },
			"type": "player"
		},
		{
			"spawn_point": { "x": 50, "y": 128 },
			"type": "player"
		},
		{
			"spawn_point": { "x": 55, "y": 64 },
			"type": "player"
		},
		{
			"spawn_point": { "x": 60, "y": 128 },
			"type": "player"
		},
		{
			"spawn_point": { "x": 70, "y": 64 },
			"type": "player"
		}
	]
}
```


### Easy to work with entity system

For a low memory overhead 2D game, each entity can be stored as a bunch of components, only using the relevant fields for that entity type:
```c
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
```
Currently implemented components are:
- Sprite component, for the state of the current animation
- Mover component, to give an entity collision with the tilemap and other mover entities.
- Player component, for the player entity specifically to be moved around with the keyboard.

Entities work well together by default through this system, I'm definitely going to reuse this system in future projects.
```c
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
```


