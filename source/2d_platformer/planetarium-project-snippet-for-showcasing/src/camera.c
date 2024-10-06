// Room has a camera
// Room gives camera a target: e.g., player, or in a cutscene: the door.
// Camera interpolates with cool tweens towards target.

#include "game.h"
#include "level.h"

void
update_camera(Room* room)
{
    
    room->camera_viewport.target.x = room->camera_target->position.x;
    room->camera_viewport.target.y = room->camera_target->position.y;

    room->camera_viewport.offset.x = (float)(game.width / 2);
    room->camera_viewport.offset.y = (float)(game.height / 2);
    room->camera_viewport.rotation = 0.0f;
    room->camera_viewport.zoom = 1.0f;
}
