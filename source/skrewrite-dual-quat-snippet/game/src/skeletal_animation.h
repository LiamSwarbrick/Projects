#ifndef SK_SKELETAL_ANIMATION_H
#define SK_SKELETAL_ANIMATION_H

#include "common_types.h"
#include "memory_arena.h"
#include "opengl_layer_types.h"

typedef struct Skin_Vertex
{
    Vector3 p;  // position
    Vector3 n;  // normal
    Vector4 c;  // colour
    Vector4 uvst;  // Two texture coordinates, fit into one vec4 on the gpu.
    u16 joint_ids[4];  // joints that influence vertex
    f32 weights[4];    // how much the joint can influence the vertex (sum of weights = 1)
}
Skin_Vertex;

typedef struct Skinlet
{
    u32 skinlet_id;

    u32 vertex_count;
    u32 index_count;
    
    Skin_Vertex* vertices;
    u32* indices;

    u32 material_id;
}
Skinlet;
#define SKIN_MATERIALS_MAX 16

typedef struct Skin
{
    u32 skinlet_count;
    // NOTE: The skinlets are grouped into smaller arrays with common a material_id.
    Grouped_Array skinlets_by_material_id;  // NOTE: Skinlets grouped into arrays of same material
    Material* materials;  // length: skinlets_by_material_ids.array_count

    // GPU:
    u32 vao;
    u32** vbos_per_skinlet_group;
    u32** ebos_per_skinlet_group;
} 
Skin;

typedef struct Joint_Animation
{
    u32 keyframe_count;
    f32* timestamps;
    Dual_Quaternion* keyframes;
}
Joint_Animation;

typedef struct Clip
{
    char name[64];
    f32 clip_length;  // seconds
    
    Joint_Animation* joint_animations;  // Each joint has seperate keyframes

    b32 loop;
}
Clip;

typedef struct Rig
{
    Skin skin;
    u32 joint_count;
    char (*joint_names)[32];

    u32 clip_count; 
    Clip* clips;
}
Rig;

typedef struct Rig_Instance
{
    Rig* rig;  // Stored in assets
    Clip* current_clip;  // Points to an animation in rig->animations
    Dual_Quaternion* current_pose;  // The interpolated pose
    f32 clip_time;

    u32* jointanim_current_keyframe_indices;  // Cache of the last used index of joint animation keyframe
}
Rig_Instance;


// Could either try and blend animations (complex) or instant change (not too bad look at majoras mask)
// https://graphics.cs.wisc.edu/Papers/2003/KG03/regCurves.pdf

void
riginst_change_clip(Rig_Instance* rig_instance, char* name)
{
    for (int i = 0; i < rig_instance->rig->clip_count; ++i)
    {
        if (cstring_compare_fixedlengths(name, rig_instance->rig->clips[i].name))
        {
            rig_instance->current_clip = &rig_instance->rig->clips[i];
            return;
        }
    }
    Assert(0);  // NOTE: name not associated to a clip of the rig
}

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

/*
static void
repose(Rig_Instance* rig_instance)
{
    // Each animation has a list of Joint_Animations, each joint animation contains seperate keyframes 
    // Go through every joint and interpolate there dq joint transform (DLB alg) in between their keyframes at clip_time
    if (rig_instance->current_clip->loop && rig_instance->clip_time > rig_instance->current_clip->clip_length)
    {
        rig_instance->clip_time = fmodf(rig_instance->clip_time, rig_instance->current_clip->clip_length);
    }
    for (u32 i = 0; i < rig_instance->rig->skeleton->joint_count; ++i)
    {
        f32 joint_keyframe_timestamp_lower, joint_keyframe_timestamp_higher;
        u32 j;
        for (j = 0; j < rig_instance->current_clip->joint_animations[i].keyframe_count; ++i)
        {
            f32 timestamp = rig_instance->current_clip->joint_animations[i].timestamps[j];
            if (timestamp >= rig_instance->clip_time)
            {
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
*/

#endif  // SK_SKELETAL_ANIMATION_H
