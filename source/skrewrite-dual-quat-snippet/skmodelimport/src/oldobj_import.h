#ifndef SKIMPORT_OBJ_IMPORT_H
#define SKIMPORT_OBJ_IMPORT_H

// NOTE: Mesh import obj format to custom format
// This OBJ importer only supports mesh and texturing
#include "memory_arena.h"
#include "common_types.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"  // https://github.com/thisistherk/fast_obj

#include <windows.h>
#include <stdio.h>

typedef struct Vertex
{
    Vector3 p;  // position
    Vector2 t;  // texcoord
    Vector3 n;  // normal
}
Vertex;

b32
export_obj_to_skm(const char* filename, u32 vertex_count, u32 index_count, Vertex* vertices, u32* indices)  // .skm
{
    // NOTE: Put data into binary format then WriteEntireFile
    // Format is simple: u32 vertex_count, u32 index_count,
    // list of vertices for bytes after index count for  vertex_count * sizeof(Vertex)  bytes,
    // list of indices for bytes after vertices for  index_count * sizeof(u32)  bytes.
    u32 output_size = (2 * sizeof(u32)) + (vertex_count * sizeof(Vertex)) + (index_count * sizeof(u32));
    void* data = VirtualAlloc(NULL, output_size, MEM_COMMIT, PAGE_READWRITE);
    void* cursor = data;

    ((u32*)cursor)[0] = vertex_count;  // TODO: <-- Check endianness doesn't mess with things here
    ((u32*)cursor)[1] = index_count;   //       <--
    cursor += 2 * sizeof(u32);

    CopyMemory(cursor, vertices, vertex_count * sizeof(Vertex));
    cursor += vertex_count * sizeof(Vertex);
    CopyMemory(cursor, indices, index_count * sizeof(u32));

    // NOTE: Now write file
    HANDLE file_handle = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle != INVALID_HANDLE_VALUE)
    {
        DWORD bytes_written;
        if (WriteFile(file_handle, data, output_size, &bytes_written, NULL) && output_size == bytes_written)
        {
            // NOTE: File wrote successfully
        }
        else
        {
            // NOTE: Failed to write to file
            CloseHandle(file_handle);
            return 0;
        }

        CloseHandle(file_handle);
    }
    else
    {
        // NOTE: Failed to create file
        return 0;
    }

    return 1;
}

int
obj_import_main(int argc, char** argv)
{
    if (argc <= 1)
    {
        printf("No command line arguments passed. Pass wavefront obj file as parameter e.g \"example.obj\".\n");
        return -1;
    }
    
    // NOTE: Wavefront obj indices start at 1, fast_obj solves this by adding a dummy (empty/zero) position v3, texcoord v2, and normal v3 at indices[0]. We want to start at index 0 so we shift everything along.
    // NOTE: The obj file also keeps positions, texcoords and normals seperate but current rendering apis don't deal with data this way, so we need to construct one list of vertices and one list of indices.
    
    fastObjMesh* obj = fast_obj_read(argv[1]);
    if (obj == NULL)
    {
        printf("Failed to read obj\n");
    }

    u32 initial_vertex_count = obj->face_count * 3;  // NOTE: 3 vertices for each face (assume triangulated faces)
    //   ^^^^^^^^^^^^ Vertex count will be lower after duplicate vertices are deleted
    //                This just gives us the upper bound of how much memory we need.
    u32 initial_index_count = obj->face_count * 3;

    Vertex* vertices = VirtualAlloc(NULL, initial_vertex_count * sizeof(Vertex), MEM_COMMIT, PAGE_READWRITE);
    u32* indices = VirtualAlloc(NULL, initial_index_count * sizeof(u32), MEM_COMMIT, PAGE_READWRITE);
    
    // NOTE: Seek through face indices, create new vertex for each face vertex, delete duplicates after
    Vector3* positions = (Vector3*)(&(obj->positions[sizeof(Vector3) / sizeof(f32)]));  // cast to V3* one ahead because of dummy v3
    Vector2* texcoords = (Vector2*)(&(obj->texcoords[sizeof(Vector2) / sizeof(f32)]));
    Vector3* normals   = (Vector3*)(&(obj->normals  [sizeof(Vector3) / sizeof(f32)]));
    u32 accumulating_vertex_count;

    for (u32 i = 0; i < obj->face_count * 3; i += 3)
    {
        fastObjIndex obj_index0 = obj->indices[i];
        fastObjIndex obj_index1 = obj->indices[i + 1];
        fastObjIndex obj_index2 = obj->indices[i + 2];

        Vertex vertex0 = { positions[obj_index0.p - 1], texcoords[obj_index0.t - 1], normals[obj_index0.n - 1] };
        Vertex vertex1 = { positions[obj_index1.p - 1], texcoords[obj_index1.t - 1], normals[obj_index1.n - 1] };
        Vertex vertex2 = { positions[obj_index2.p - 1], texcoords[obj_index2.t - 1], normals[obj_index2.n - 1] };

        vertices[i]     = vertex0;
        vertices[i + 1] = vertex1;
        vertices[i + 2] = vertex2;

        // NOTE: Before deleting duplicate vertices and indices the indices simply are: 0, 1, 2, ...
        indices[i]     = i;
        indices[i + 1] = i + 1;
        indices[i + 2] = i + 2;
    }
    
    /*  OLD WRONG (this took ages to figure out):
    for (u32 face = 0; face < obj->face_count; ++face)
    {
        fastObjIndex obj_index0 = obj->indices[1 + (face * 3)];  // NOTE: Dummy index at 0
        fastObjIndex obj_index1 = obj->indices[2 + (face * 3)];
        fastObjIndex obj_index2 = obj->indices[3 + (face * 3)];
        // NOTE: Vertices that form the triangle for this this face
        Vertex vertex0 = { positions[obj_index0.p], texcoords[obj_index0.t], normals[obj_index0.n] };
        Vertex vertex1 = { positions[obj_index1.p], texcoords[obj_index1.t], normals[obj_index1.n] };
        Vertex vertex2 = { positions[obj_index2.p], texcoords[obj_index2.t], normals[obj_index2.n] };
        initial_vertices[face * 3]       = vertex0;
        initial_vertices[(face * 3) + 1] = vertex1;
        initial_vertices[(face * 3) + 2] = vertex2;
        // NOTE: These indices are  0, 1, 2, ...  because duplicate vertices haven't been stripped yet.
        initial_indices[face * 3] = face * 3;
        initial_indices[(face * 3) + 1] = (face * 3) + 1;
        initial_indices[(face * 3) + 2] = (face * 3) + 2;
    }*/
    u32 vertex_count = initial_vertex_count;
    u32 index_count = initial_index_count;
    
    for (u32 i = 0; i < index_count; ++i)
    {
        Vertex vertexi = vertices[indices[i]];
        for (u32 j = i+1; j < index_count; ++j)
        {
            Vertex vertexj = vertices[indices[j]];

            if (v3_compare(vertexi.p, vertexj.p) && v2_compare(vertexi.t, vertexj.t) && v3_compare(vertexi.n, vertexj.n))
            {
                // Delete duplicate vertex
                for (u32 k = indices[j]; k < vertex_count-1; ++k)
                {
                    vertices[k] = vertices[k+1];  // Remove vertex
                }
                vertex_count -= 1;

                for (u32 k = 0; k < index_count; ++k)
                {
                    // Update indices accounting for removed vertex
                    if (indices[k] > indices[j])
                    {
                        indices[k] -= 1;
                    }
                }
                indices[j] = indices[i];
            }
        }
    }
    
    /* OLD WRONG AGAIN
    // NOTE: Because some vertices are duplicates, some indices represent the same vertex.
    // Create new array of unique vertices with new indices array to match
    u32 vertex_count = initial_vertex_count;
    u32 index_count = initial_index_count;
    Vertex* vertices = VirtualAlloc(NULL, initial_vertex_count * sizeof(Vertex), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    u32* indices = VirtualAlloc(NULL, initial_index_count * sizeof(u32), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CopyMemory(vertices, initial_vertices, vertex_count * sizeof(Vertex));
    CopyMemory(indices, initial_indices, index_count * sizeof(u32));
    for (u32 i = 0; i < index_count; ++i)
    {
        u32 indexi = indices[i];
        Vertex vertexi = vertices[indexi];
        for (u32 j = 0; j < vertex_count; ++j)
        {
            if (i == j) continue;

            u32 indexj = indices[j];
            Vertex vertexj = vertices[indexj];

            if (v3_compare(vertexi.p, vertexj.p) && v2_compare(vertexi.t, vertexj.t) && v3_compare(vertexi.n, vertexj.n))
            {
                // NOTE: Delete duplicate vertex
                for (u32 k = indexj; k < vertex_count; ++k)
                {
                    vertices[k] = vertices[k+1];  // Shift everything down removing duplicate
                }
                indices[j] = indices[i];
                vertex_count -= 1;
            }
        }
    }*/
    
    u32 out_filename_strlen = cstring_length(argv[1]);  // NOTE: Same output file name but with .obj switch to .raf
    Memory_Arena out_filename_arena = arena_init(VirtualAlloc(NULL, out_filename_strlen + sizeof(String), MEM_COMMIT, PAGE_READWRITE), out_filename_strlen);
    String out_filename = arena_push_string(&out_filename_arena, argv[1]);
    out_filename.data[out_filename.length - 3] = 's';
    out_filename.data[out_filename.length - 2] = 'k';
    out_filename.data[out_filename.length - 1] = 'm';
    
    export_obj_to_skm(out_filename.data, vertex_count, index_count, vertices, indices);

    return 0;
}

#endif  // SKIMPORT_OBJ_IMPORT_H
