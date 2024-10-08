#ifndef SK_INPUT_H
#define SK_INPUT_H

#include "common_types.h"
#include "program_options.h"

typedef struct Input
{
    // MOUSE:
    b32 mouse_captured;
    b32 mouse_hidden;

    Vector2i mouse_position;
    Vector2i mouse_relative;
    b32 mouse_1;  // NOTE Left click
    b32 mouse_2;  // NOTE: Right click
    b32 mouse_3;  // NOTE: Middle click
    b32 mouse_4;  // NOTE: Back aka xbutton1
    b32 mouse_5;  // NOTE: Forward aka xbutton2
    Vector2i mouse_1_click_position;
    Vector2i mouse_2_click_position;

    // KEYBOARD:
    int mod_alt;
    b8 keys[0xFF];
}
Input;

#endif  // SK_INPUT_H
