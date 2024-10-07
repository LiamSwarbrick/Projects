# My Projects

### TODO: Godot game jam stuff, jack compiler, and other interesting things

I (Liam) am currently in third year at University of Leeds studying a Meng/Bsc in Computer Science with High Performance Computer Graphics and Games Engineering.

I'm currently doing a third year project in High Performance Computer Graphics. Here are some previous and ongoing projects of mine.


## Voxel Game in C with OpenGL 4.5 from scratch on the Win32API
![Early screenshot from my voxel game](files/blockgame-early-screenshot.PNG)

Voxel rendering with chunking, texture arrays and mesh-culling optimisations. My most recent project so it's at an early stage. I intend to add world generation, and need to add a faster chunk mapping system.

[Feel free to peruse the source code to my program here if you love some C programming like me!](source\c_blockgame_september_snippet\src)

Programmed so that the Windows API layer is easy simple to port and swap with a cross-platform library. Using no libraries was great for learning. I implemented my own math library originally ([source here](source/c_blockgame_september_snippet/old_custom_math_src\src_oldcustommath/include/basic_types.h)) but swapped it out for cglm, since I could trust its correctness so I could rule out any rendering errors being due to my maths function.

RenderDoc and OpenGL 4.5's debugging API were very useful for GPU debugging, which I needed to do a lot of, especially when building and uploading mesh data for each voxel chunk.


## 2D Platformer Engine in C with Raylib [(read here)](2d_c_engine.md)

![Cropped Gameplay of my 2D platformer engine](files/planetarium-camera-and-entities-cropped.gif)
<img src="files/planetarium-collision-box.PNG" alt="Collision box from aseprite" width="33%">
<img src="files/planetarium-initial-tilemap-editor.gif" alt="Tile editor" width="36%">

[Link to the markdown page to read more](2d_c_engine.md) <--
- Custom sprite animation and asset system integrated with the Aseprite pixelart program for a simple fun asset pipeline.
- Integrated tilemap editor, serialising levels with json.

At some point I want to create a full length 2D story/platforming game this way to release on Steam since I love 2D indie games.

- [Level creation and serialisation](2d_c_engine.md#level-creation-and-serialisation)
- [Sprite system loading Aseprite files](2d_c_engine.md#sprite-system-loading-aseprite-files)
- [Easy to work with entity system](2d_c_engine.md#easy-to-work-with-entity-system)


## Compiler for Java-like language called 'Jack'

TODO

## 2019 Godot-Wild Jam #5 Winner: My Teeth-Grinding Precision Platformer

For the theme of "One-Control", in just over a week I made an entry for the January 2019 [Godot-Wild Jam #5](https://itch.io/jam/godot-wild-jam-5/rate/358355)!

<!-- ![Godot Jam Game screenshot](files/liam_godot-wild-5-winner.png) -->
<img src="files/liam_godot-wild-5-winner.png" alt="Godot Jam Game screenshot" width="75%">

Here's some low-res compressed gif gameplay of it:

<img src="files/gwj5-mushroomgame.gif" alt="Godot Wild Game Gif" width="50%">

The 9-day long Game Jam required the use of the Godot Game engine (nowadays this engine has gotten a lot of additions and popularity since 2019). I had fun with designing the levels to be built perfectly around the mushroom player controller, requiring skill and precision (taking up peoples whole afternoons to beat all the levels). I also had fun with sound design, making sound effects, respawn transitions, and adding fractal-brownian-motion fog for atmosphere.

The community behind this jam was really great, and people gave lots of lovely feedback! (You can see it [here on my entry page for the jam](https://itch.io/jam/godot-wild-jam-5/rate/358355))

<img src="files/godot-wild-5-feedback1.PNG" alt="Godot Wild Game feedback" width="70%">

<img src="files/godot-wild-5-feedback2.PNG" alt="Godot Wild Game feedback" width="70%">


### C\# 2018

Game tooling:
Created a game drop down console with command processing. (C\# 2018)



### C programming
2019
- OpenGL testing phong shading-
- Unfinished skeletal animation programming (tried implementing a dual quaternion vertex skinning algorithm)
- Game boy emulator (never finished)
