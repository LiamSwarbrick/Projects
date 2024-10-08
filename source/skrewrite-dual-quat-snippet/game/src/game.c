#include "common_types.h"
#include "sakuren.h"
#include "input.h"
#include "memory_arena.h"
#include "opengl_layer.h"

void
game_resize_callback(int width, int height)
{
    glViewport(0, 0, width, height);
}

void
game_update_and_render(Platform* platform_)
{
    platform = platform_;
    Assert(sizeof(Game) <= platform->permanent_storage_size);

    game = (Game*)platform->permanent_storage;
    if (!platform->initialized)
    {
        platform->initialized = 1;
    
        
        platform->resize_callback = game_resize_callback;
        game->permanent_arena = arena_init(platform->permanent_storage, platform->permanent_storage_size);
        game->scene_arena = arena_init(game->permanent_arena.memory, game->permanent_arena.memory_left);
        game->asset_arena = arena_init(platform->asset_storage, platform->asset_storage_size);
        game->frame_arena = arena_init(platform->transient_storage, platform->transient_storage_size);

        set_window_title(arena_push_string(&game->frame_arena, "skrewrite | %s", glGetString(GL_VERSION)).data);

        platform->fullscreen = 0;

        // NOTE: Set the OpenGL debug callback to opengl_callback from plvr_gl.h
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, 1);

        glViewport(0, 0, platform->window_width, platform->window_height);
        
        glFrontFace(GL_CW);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);  // NOTE: Premultiplied alpha

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // NOTE: Wireframe mode
        glPatchParameteri(GL_PATCH_VERTICES, 3);

        // TEMP OPENGL PROGRAM:
        // NOTE: Create shader program
        char* vertex_string = DEBUG_read_entire_file("game/data/shaders/rigged_mesh_vertex_shader.glsl");
        char* fragment_string = DEBUG_read_entire_file("game/data/shaders/material_fragment_shader.glsl");
        game->shader = compile_shader_program(vertex_string, fragment_string, "Rig OGL4.5 Shader");
        DEBUG_free_file_memory(vertex_string);
        DEBUG_free_file_memory(fragment_string);
        
        
        // NOTE: Load texture
        game->tnt_texture = load_texture("game/data/textures/tnt_side.png");

        glCreateBuffers(1, &game->ubo_0);
        load_rig("...", &game->asset_arena, &game->rig);
        game->rig_instance.rig = &game->rig;
        riginst_change_clip(&game->rig_instance, "TODO_CLIP_NAME_TO_PLAY !!!!!!!!!!!!!!");
    }
    arena_empty(&game->frame_arena);
    
    char* new_title = arena_push_string(&game->frame_arena, "Skren  res(w:%d, h:%d),  fps(%f/s)", platform->window_width, platform->window_height, 1.0 / platform->delta_time).data;
    set_window_title(new_title);

    // Update:
    
    if (platform->input.keys[VK_UP] && !platform->last_input.keys[VK_UP])
    {
        platform->target_fps += 10;
    }
    else if (platform->input.keys[VK_DOWN] && !platform->last_input.keys[VK_DOWN])
    {
        platform->target_fps -= 10;
    }

    glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Matrix4 projection = m4_perspective(0.01f, 100.0f, PI / 2.0f, (f32)platform->window_width / (f32)platform->window_height);
    Matrix4 view = m4_view((Vector3){ 0.0f, 0.0f, -1.0f }, (Vector3){ 0.0f, 0.0f, 1.0f }, (Vector3){ 0.0f, 1.0f, 0.0f });

    Matrix4 model = M4_IDENTITY_INIT;
    // model = m4_mul(m4_rotate_xyz((Vector3){ 0.0f, PI/2.0f, PI/2.0f }), model);
    model = m4_mul(m4_rotate((Vector3){ 0.0f, 1.0f, 0.0f }, 0.2f * (f32)platform->time), model);
    model = m4_mul(m4_scale(model, (Vector3){ 1.0f, 1.0f, 1.0f }), model);
    model = m4_mul(m4_translate(M4_IDENTITY, (Vector3){ 0.0f, 0.0f, -5.0f }), model);
    Matrix4 mvp = m4_mul(m4_mul(projection, view), model);

    glUseProgram(game->shader.id);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, game->shader.ubo_0);

    glNamedBufferSubData(game->ubo_0, (GLintptr)0, sizeof(Dual_Quaternion) * game->rig_instance.rig.joint_count, game->rig_instance.current_pose);

    for (u32 m = 0; m < game->rig_instance.rig.skin.skinlets_by_material_id.array_count; ++m)  // foreach material in rig's skin
    {
        // glNamedBufferSubData for material uniform buffer object

        glMultiDrawElementsIndirect(GL_TRIANGLES, )
    }

    /*
    glUseProgram(game->shader.id);
    glBindVertexArray(game->mesh.vao);
    
    glProgramUniformMatrix4fv(game->shader.id, 0, 1, GL_FALSE, (float*)mvp.m);
    glBindTextureUnit(0, game->tnt_texture.id);
    glDrawElements(GL_TRIANGLES, game->mesh.index_count, GL_UNSIGNED_INT, 0);
    */
    swap_buffers();
}
