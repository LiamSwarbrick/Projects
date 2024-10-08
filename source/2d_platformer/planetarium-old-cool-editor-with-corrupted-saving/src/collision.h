#ifndef PLANETARIUM_COLLISION_H
#define PLANETARIUM_COLLISION_H

#include <raylib.h>

#define COLLISION_TYPE_BOX 0

typedef struct Collision
{
    int type;
    Rectangle box;
    // Circle circle;
}
Collision;

#endif  // PLANETARIUM_COLLISION_H
