#ifndef SK_SKELETAL_ANIMATION_H
#define SK_SKELETAL_ANIMATION_H

#include "common_types.h"
#include "memory_arena.h"
#include "opengl_layer_types.h"

/*
NOTES:
bone = joint
- local space / bone space means the transform reletive to its parent bone
- bind transform is the origional/default transform of the bone

TODO:

*/

#define VERTEX_MAX_BONE_INFLUENCE 4  // fits into one GPU register (vec4: 4 *4bytes)

typedef struct Skeleton_Vertex
{
    Vector3 p;  // position
    Vector2 t;  // texcoords
    Vector3 n;  // normal
    u16 bone_ids[VERTEX_MAX_BONE_INFLUENCE];  // bones that influence vertex
    f32 weights[VERTEX_MAX_BONE_INFLUENCE];   // how much the bone can influence the vertex
    // NOTE: The sum of the weights of bones that influence a single vertex need to sum up to one else you'll end up with strange results
}
Skeleton_Vertex;

typedef struct Bone
{
    char* name;
    u32 parent_index;
    Matrix4 local_matrix;  // the bone described by transformation from its parent bone
    Matrix4 inv_bind_matrix;
}
Bone;

typedef struct Bone_Transform  // NOTE: IN FUTURE USE DUAL QUATERNIONS OR MOTORS
{
    Vector3 position;
    Versor rotation;
}
Bone_Transform;

typedef struct Skeleton_Key_Frame
{
    u32 bone_transform_count;
    Bone_Transform* bone_transforms;
    f32 time_stamp;
}
Skeleton_Key_Frame;

typedef struct Skeleton_Animation
{
    u32 key_frame_count;
    Skeleton_Key_Frame* key_frames;  // ordered by time_stamp
    b32 loop;
}
Skeleton_Animation;

typedef struct Skeleton_Mesh
{
    u32 vertex_count;
    u32 index_count;

    Skeleton_Vertex* vertices;
    u32* indices;

    u32 bone_count;
    Bone* bones;

    // GPU:
    u32 vao, vbo, ebo;
}
Skeleton_Mesh;

static macro void
skeleton_repose(Skeleton_Mesh mesh, Skeleton_Animation anim, f32 anim_time)
{
    // NOTE: Find the two keyframe time stamps that anim_time is between
    // Use property that anim.key_frames is ordered by time_stamp
    Skeleton_Key_Frame kf0, kf1;
    f32 kf0_weight;  // weight = 1 - kf0_weight
    for (int i = 0; i < anim.key_frame_count; ++i)
    {
        if (i == anim.key_frame_count - 1)
        {
            if (anim.loop)  // NOTE: Will only loop seamlessly if last key frame is the same pose as the first
            {
                kf0 = anim.key_frames[0];
                kf1 = anim.key_frames[1];
                kf0_weight = (anim_time - anim.key_frames[i].time_stamp) / (kf1.time_stamp - kf0.time_stamp);
            }
            else
            {
                kf0 = anim.key_frames[i];
                kf1 = kf0;
                kf0_weight = 0.0f;
            }
        }

        if (anim.key_frames[i].time_stamp <= anim_time && anim_time < anim.key_frames[i+1].time_stamp)
        {
            kf0 = anim.key_frames[i];
            kf1 = anim.key_frames[i + 1];
            kf0_weight = (anim_time - kf0.time_stamp) / (kf1.time_stamp - kf0.time_stamp);
            break;
        }
    }


    // go through all the bones and transform them by the anim
}

#endif  // SK_SKELETAL_ANIMATION_H
