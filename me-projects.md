# My Projects

I (Liam) am currently in third year at University of Leeds studying Meng/Bsc Computer Science with High Performance Computer Graphics and Games Engineering.

I'm currently doing a third year project in High Performance Computer Graphics. Here are some previous and ongoing projects of mine.

### Voxel Game in C with OpenGL 4.5 from scratch on the Win32API
![](files/blockgame-early-screenshot.PNG)

Voxel rendering with chunking, texture arrays and face-culling optimisations. My most recent project so at an early stage. I intend to add world generation, and need to add a faster chunk mapping system.

Programmed so that the Windows API layer is easy to swap out with a cross-platform library. Using no libraries was great for learning. I implemented my own math library originally ([source here](source/c_blockgame_september_snippet/old_custom_math_src\src_oldcustommath/include/basic_types.h)) but swapped it out for cglm to rule out any graphical errors being due to my maths function.

RenderDoc and OpenGL 4.5's debugging API were very useful for GPU debugging, which I needed to do a lot of for this, especially when rendering faces for each voxel chunk.

### [2D Platformer Engine in C with Raylib](2d_c_engine.md)

![Cropped Gameplay of my 2D platformer engine](files/planetarium-camera-and-entities-cropped.gif)

[Link to the markdown page for it](2d_c_engine.md) <--
- Custom sprite animation and asset system integrated with the Aseprite pixelart program for a simple fun asset pipeline.
- Integrated tilemap editor, serialising levels with json.

At some point I want to create a full length 2D story/platforming game this way to release on Steam since I love 2D indie games.

- [Level creation and serialisation](2d_c_engine.md#level-creation-and-serialisation)
- [Sprite system loading Aseprite files](2d_c_engine.md#sprite-system-loading-aseprite-files)
- [Easy to work with entity system](2d_c_engine.md#easy-to-work-with-entity-system)


C\# 2018

Game tooling:
Created a game drop down console with command processing. (C\# 2018)



C programming
2019
- OpenGL testing phong shading-
- Unfinished skeletal animation programming (tried implementing a dual quaternion vertex skinning algorithm)
- Game boy emulator (never finished)