# My Projects
**Liam Swarbrick**  
_Looking for a summer internship / summer job_

I am currently in my third year at the **University of Leeds** studying a **MEng/BSc in Computer Science with High Performance Computer Graphics and Games Engineering**.

I'm actively developing my thesis project **"Integrating Linearly-Transformed Cosine based Area Lights into a Clustered Forward Renderer"**. [(Progress update below.)](#thesis-integrating-linearly-transformed-cosine-based-area-lights-into-a-clustered-forward-renderer-progress-update)

Here are some of my projects; I'm passionate about **computer graphics and games engineering**. After three summer harvests farming for plant breeders, I have completed photosynthesis and look forward to working in computers.

****
- [My Projects](#my-projects)
  - [Thesis "Integrating Linearly-Transformed Cosine based Area Lights into a Clustered Forward Renderer" Progress Update](#thesis-integrating-linearly-transformed-cosine-based-area-lights-into-a-clustered-forward-renderer-progress-update)
      - [DONE: Custom C OpenGL 4.6 glTF scene renderer PBR Base Color only](#done-custom-c-opengl-46-gltf-scene-renderer-pbr-base-color-only)
      - [Ongoing: PBR Metallic roughness, Area Lights, Clustered Shading, Area light assignment algorithm](#ongoing-pbr-metallic-roughness-area-lights-clustered-shading-area-light-assignment-algorithm)
  - [Voxel Game in C with OpenGL 4.5 from scratch on the Win32API](#voxel-game-in-c-with-opengl-45-from-scratch-on-the-win32api)
  - [2D Platformer Engine in C with Raylib](#2d-platformer-engine-in-c-with-raylib)
  - [Compiler for Java-like language 'Jack' written in C](#compiler-for-java-like-language-jack-written-in-c)
  - [2019 Game-Jam Winner: My Precision Platformer for the theme "One Control"](#2019-game-jam-winner-my-precision-platformer-for-the-theme-one-control)
  - [Library Website in Python-Flask](#library-website-in-python-flask)
  - [My Godot game that ran on the Nintendo Switch](#my-godot-game-that-ran-on-the-nintendo-switch)
  - [Snippet of my older unfinished but of interest projects](#snippet-of-my-older-unfinished-but-of-interest-projects)
    - [Dual-Quaternion vertex skinning implementation I couldn't render (2020)](#dual-quaternion-vertex-skinning-implementation-i-couldnt-render-2020)
    - [Unfinished C Game-Boy Emulator (2019)](#unfinished-c-game-boy-emulator-2019)
    - [C# Game drop-down console (2018)](#c-game-drop-down-console-2018)


## Thesis "Integrating Linearly-Transformed Cosine based Area Lights into a Clustered Forward Renderer" Progress Update

This october/november I've begun my thesis project **"Integrating Linearly-Transformed Cosine based Area Lights into a Clustered Forward Renderer"**, I'm developing my own OpenGL 4.6 renderer for it in C and am currently working on rendering glTF scene files with a PBR renderer.

Once that is done I will work on area lights - which are physically based polygonal lights, and I will design a way to assign them to spatial clusters in order to integrate clustered shading which will drastically reduce the large GPU throughput that comes from forward rendering lots of lights and thus render scenes with potentially thousands of these lights in real-time.

#### DONE: Custom C OpenGL 4.6 glTF scene renderer PBR Base Color only

Output radiance is gamma-corrected to sRGB from linear color-space as it should be. Textures that represent color are converted from sRGB to linear space on load with `GL_SRGB8_ALPHA8`.

<img src="files/12nov-sponza-basecolor-simple-diffuse-directional-(smallpng).PNG">
<img src="files/9nov2024-gltf_with_normals.PNG" width=50%>

#### Ongoing: PBR Metallic roughness, Area Lights, Clustered Shading, Area light assignment algorithm

## Voxel Game in C with OpenGL 4.5 from scratch on the Win32API
![Early screenshot from my voxel game](files/blockgame-early-screenshot.PNG)

Voxel rendering with 3D chunking, texture arrays and mesh-culling optimisations. My most recent project so it's at an early stage. I intend to add proper world generation, and need to add a faster chunk hashing system.

My 3D chunk storage allows infinite height worlds which opens up more interesting world generation possibilities - sky islands, planets, deep sea trenches, the possibilities are endless.

[Feel free to peruse the source code to my program here if you also like some C programming](source/c_blockgame_september_snippet/src)

Programmed so that the Windows API layer is straight forward to swap with a cross-platform library for Linux. Using no libraries was great for learning (hence not using GLFW). I implemented my own math library originally ([which you can see here](source/c_blockgame_september_snippet/old_custom_math_src/src_oldcustommath/include/basic_types.h)) but swapped it out for [cglm](https://github.com/recp/cglm) since I could trust its mathematical correctness when debugging rendering errors.

RenderDoc and OpenGL 4.5's debugging API were very useful for GPU debugging, which I needed to do a lot of, especially for building and uploading the mesh data of each voxel chunk.

## 2D Platformer Engine in C with Raylib

![Cropped Gameplay of my 2D platformer engine](files/planetarium-camera-and-entities-cropped.gif)
<img src="files/planetarium-collision-box.PNG" alt="Collision box from aseprite" width="33%">
<img src="files/planetarium-room-editor-fun.gif" alt="Tile editor" width="37%">
<!-- <img src="files/planetarium-room-editor.gif" alt="Tile editor" width="40%"> -->

- Custom sprite animation and asset system integrated with the Aseprite pixelart program for a simple fun asset pipeline.
- Integrated tilemap editor, serialising levels with json.

At some point I want to create a full length 2D story/platforming game this way to release on Steam since I love 2D indie games.

[Link to the markdown page to read about it](2d_c_engine.md) **<-**
- [Level creation and serialisation](2d_c_engine.md#level-creation-and-serialisation)
- [Sprite system loading Aseprite files](2d_c_engine.md#sprite-system-loading-aseprite-files)
- [Easy to work with entity system](2d_c_engine.md#easy-to-work-with-entity-system)

## Compiler for Java-like language 'Jack' written in C

This project was for a University coursework, written in C using recursive-descent and it passed all testing and edge cases, receiving max marks. The input is a folder of .jack source files and compiles to virtual machine code for the Hack computer.

<img src="files/jack_compiler_pong.PNG" alt="My compiler's output for Nand2Tetris' Pong jack program" width="75%">

## 2019 Game-Jam Winner: My Precision Platformer for the theme "One Control"

For the theme of "One-Control", in just over a week I made an entry for the January 2019 [Godot-Wild Jam #5](https://itch.io/jam/godot-wild-jam-5/rate/358355)!

<!-- ![Godot Jam Game screenshot](files/liam_godot-wild-5-winner.png) -->
<img src="files/gwj5-mushroomgame.gif" alt="Godot Wild Game Gif" width="50%">

My solo-entry was the result of 8-days of working in the Godot game engine (which has come a long way since 2019). I programmed a weighty one-control movement system, the physics feel responsive and precise as you build momentum. I had lots of fun drawing and animating the pixel art, and meticulously designed the levels perfectly around the mushroom player's movements, requiring skill and precision (taking up peoples afternoons to beat all the levels). I also had success with the sound design, respawn transitions, and adding fractal-brownian-motion fog for atmosphere.

<img src="files/godot-wild-5-feedback1.PNG" alt="Godot Wild Game feedback" width="100%">
<img src="files/liam_godot-wild-5-winner.png" alt="Godot Jam Game screenshot" width="100%">
<img src="files/godot-wild-5-feedback2.PNG" alt="Godot Wild Game feedback" width="100%">

The community behind this jam was really great and gave lots of lovely feedback! ([You can see it here on my entry page for the jam](https://itch.io/jam/godot-wild-jam-5/rate/358355))



## Library Website in Python-Flask

This project was for a University coursework. The site dynamically loads books from the server, has a secure log in system, book reviews and rating system, user's personal library.
The site is accessible, for instance in an accessibility pass the colours and contrasts were adjusted to be more colour-blind friendly, and the site passed checks with WAVE (A web accessability evaluation tool).

Users, Books, and Reviews are stored in a SQL database. [Here is the models python file for instance](source/library-site-source-snippet/app/models.py)

<img src="files/libsite-bookpage.png" alt="Library website example book page" width="75%">
<img src="files/libsite-librarypage.png" alt="Library website library page" width="75%">


## My Godot game that ran on the Nintendo Switch

For the 100-hour long Extra Credits Game Jam #3 (Feb 2019), I made a grapple hook game in the Godot Engine. I went for a game-boy style colour restriction since it was faster to make the assets that way in such a short time-span.
_Since 2019 to the time of writing this, it should still be on [liamswarbrick.github.io](https://liamswarbrick.github.io), an updated version at least, which is slightly longer (but without the end screen) and slightly tighter controls than the original downloadable version._

<img src="files/astro0gameplay.gif" alt="Astro0 platformer gif" width="75%">

The Godot engine founder Ariel Manzur emailed me asking if I'd like to see a build on the switch, I quickly added gamepad and touch screen controls and was overjoyed to see it running since the Switch felt so new, I don't recall even seeing one in person at that point, The touch-screen controls were janky on there since I of course couldn't test it before sending a build over.

<img src="files/Astro0onSwitch.png" alt="Astro0 on Nintendo Switch" width="75%">

<!-- ## C\# 2018, Game drop-down console with command processing



## C programming 2019
2019
- OpenGL testing phong shading-
- Unfinished skeletal animation programming (tried implementing a dual quaternion vertex skinning algorithm)
- Game boy emulator (never finished) -->

## Snippet of my older unfinished but of interest projects
I have many older and unfinished projects in all sorts of languages: Python, C#, Haxe, Rust etc. Here's some random examples...

### Dual-Quaternion vertex skinning implementation I couldn't render (2020)

For my A-Level Computer Science project I went for a far too challenging project for my 16 year old self but it helped me so much more in the years to come than any boring project I could've finished.

I wrote an OpenGL 4.5 renderer that could load obj files, and I wanted to try skeletal animation. Naively after learning about the basics, I went straight to trying to implemented a more advanced technique from [this paper](https://team.inria.fr/imagine/files/2014/10/skinning_dual_quaternions.pdf) "Skinning with Dual Quaternions" that used dual-quaternion transforms instead of matrices, and while I did learn a lot about skinning algorithms researching it. Once I had written a lot of C code for it (even doing premature optimisations like caching joint keyframes), I realised that I couldn't source assets with skeletal animations stored in such a format without writing a custom exporter for Blender. I now know better and would have just gone with the simpler algorithm until performance or rendering quality called for more.

See my implementation in the C header file [skeletal_animation.h](source/skrewrite-dual-quat-snippet/game/src/skeletal_animation.h) and the shader code [rigged_mesh_vertex_shader.glsl...](source/skrewrite-dual-quat-snippet/game/data/shaders/rigged_mesh_vertex_shader.glsl)

```c
...  // Lots of data structure definitions

typedef struct Rig_Instance
{
    Rig* rig;  // Stored in assets
    Clip* current_clip;  // Points to an animation in rig->animations
    Dual_Quaternion* current_pose;  // The interpolated pose
    f32 clip_time;

    u32* jointanim_current_keyframe_indices;  // Cache of the last used index of joint animation keyframe
}
Rig_Instance;

static void
repose_cached(Rig_Instance* rig_instance)
{
    Assert(rig_instance->current_clip != NULL);

    b32 reset_cache_indices = 0;
    if (rig_instance->current_clip->loop && rig_instance->clip_time > rig_instance->current_clip->clip_length)
    {
        rig_instance->clip_time = fmodf(rig_instance->clip_time, rig_instance->current_clip->clip_length);
        reset_cache_indices = 1;
    }
    for (u32 i = 0; i < rig_instance->rig->skeleton->joint_count; ++i)
    {
        u32 cached_index = reset_cache_indices ? 0 : rig_instance->jointanim_current_keyframe_indices[i];
        f32 joint_keyframe_timestamp_lower = rig_instance->current_clip->joint_animations[i].timestamps[cached_index];
        f32 joint_keyframe_timestamp_higher = rig_instance->current_clip->joint_animations[i].timestamps[cached_index + 1];

        u32 j;
        for (j = cached_index; j < rig_instance->current_clip->joint_animations[i].keyframe_count; ++i)
        {
            f32 timestamp = rig_instance->current_clip->joint_animations[i].timestamps[j];
            if (timestamp >= rig_instance->clip_time)
            {
                rig_instance->jointanim_current_keyframe_indices[i] = j;  // NOTE: Cache new index
                joint_keyframe_timestamp_lower = timestamp;
                
                if (j == rig_instance->current_clip->joint_animations[i].keyframe_count - 1)
                {
                    joint_keyframe_timestamp_higher = timestamp;
                    break;
                }

                joint_keyframe_timestamp_higher = rig_instance->current_clip->joint_animations[i].timestamps[j + 1];

                break;
            }
        }

        // NOTE: t is the percent between keyframe timestamps to interpolate
        f32 t = (rig_instance->clip_time - joint_keyframe_timestamp_lower) / (joint_keyframe_timestamp_higher - joint_keyframe_timestamp_lower);
        // Interpolate dual quaternions weighted by t*q1, (1-t)*q2  (DLB algorithm)
        Dual_Quaternion result = dq_normalize(dq_add(dq_scale(rig_instance->current_clip->joint_animations[i].keyframes[j], t),
                                                     dq_scale(rig_instance->current_clip->joint_animations[i].keyframes[j+1], 1.0f - t)));
        rig_instance->current_pose[i] = result;
    }
}
```

```glsl
#version 450 core

... // Dual Quaternion stuff

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec4 vertex_colour;
layout (location = 3) in vec4 vertex_uvst;
layout (location = 4) in ivec4 joint_ids;
layout (location = 5) in vec4 weights;

out vec3 normal;
out vec4 colour;
out vec4 uvst;

layout (location = 0) uniform mat4 mvp;
layout (binding = 0) uniform Dual_Quaternion joint_transforms[64];
/* EXAMPLE:
layout (binding = 1) uniform material_buffer
{
    vec4 material_diffuse;
    vec4 material_emissive;
};*/

// look at shader storage blocks: https://www.geeks3d.com/20140704/tutorial-introduction-to-opengl-4-3-shader-storage-buffers-objects-ssbo-demo/
// glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ...);
void
main(void)
{
    Dual_Quaternion b;
    b = dq_normalize(sum_of_4_dqs(
                      dq_scale(weights.x, joint_transforms[joint_ids.x]),
                      dq_scale(weights.y, joint_transforms[joint_ids.y]),
                      dq_scale(weights.z, joint_transforms[joint_ids.z]),
                      dq_scale(weights.w, joint_transforms[joint_ids.w])
                      ));

    vec3 t = vec3( 2.0 * (-b.d.w * b.r.x + b.d.x * b.r.w - b.d.y * b.r.z + b.d.z * b.r.y),
               2.0 * (-b.d.w * b.r.y + b.d.x * b.r.z + b.d.y * b.r.w - b.d.z * b.r.x),
               2.0 * (-b.d.w * b.r.z + b.d.x * b.r.y + b.d.y * b.r.x + b.d.z * b.r.w) );
    
    mat4 m;
    float xx = 2 * b.r.x * b.r.x;
    float yy = 2 * b.r.y * b.r.y;
    float zz = 2 * b.r.z * b.r.z;
    float xy = 2 * b.r.x * b.r.y;
    float wz = 2 * b.r.w * b.r.z;
    float xz = 2 * b.r.x * b.r.z;
    float wy = 2 * b.r.w * b.r.y;
    float yz = 2 * b.r.y * b.r.z;
    float wx = 2 * b.r.w * b.r.x;
    m[0][0] = 1 - yy - zz;
    m[0][1] = xy + wz;
    m[0][2] = xz - wy;
    m[1][0] = xy - wz;
    m[1][1] = 1 - xx - zz;
    m[1][2] = yz + wx;
    m[2][0] = xz + wy;
    m[2][1] = yz - wx;
    m[2][2] = 1 - xx - yy;
    m[3] = vec4(t, 1.0);

    vec4 new_position = m * vec4(vertex_pos, 1.0);
    vec4 new_normal = m * vec4(vertex_normal, 0.0);

    gl_Position = mvp * new_position;
    normal = new_normal;
    colour = vertex_colour;
    uvst = vertex_uvst;
}
```

### Unfinished C Game-Boy Emulator (2019)

I was very interested in game console emulation, but never finished implementing all the instructions for the Game-Boy's processor. I'll definitely do some more emulator development in the future.

<img src="files/nanogb-github.PNG" width="50%">

```c
typedef u8 MMU[0xFFFF];

// cpu (Sharp LR35902 core @ 4.19 MHz, similar to the Z80)
typedef struct CPU
{
    union { struct { u8 f; u8 a; }; u16 af; };  // A, F
    union { struct { u8 c; u8 b; }; u16 bc; };  // B, C
    union { struct { u8 e; u8 d; }; u16 de; };  // D, E
    union { struct { u8 l; u8 h; }; u16 hl; };  // H, L
    u16 pc;  // program counter
    u16 sp;  // stack pointer

    MMU mmu;  // MMU used to map virtual addresses with physical addresses

    u64 t_clock;  // every machine cycle takes exactly 4 T states
    u8 delta_t_clock;  // cycles for last instrution
    
    struct
    {
        u16 mode;
        u16 mode_clock;
        u16 line;  // current scanline
    } video;
}
CPU;
```

### C\# Game drop-down console (2018)

Made in C# and Raylib, command processing was implemented after this old video, intended to be used for game debugging.

<img src="files/csharp-drop-down-console.gif" width="75%">
