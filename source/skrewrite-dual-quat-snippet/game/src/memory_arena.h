#ifndef SK_MEMORY_ARENA_H
#define SK_MEMORY_ARENA_H

#include "common_types.h"
#include <stdarg.h>  // varargs
#include <stdio.h>   //

typedef struct Memory_Arena
{
    void* memory;
    size_t size;
    size_t cursor;
    size_t memory_left;
}
Memory_Arena;

static void
zero_memory(void* memory, size_t size)
{
    u8* byte = (u8*)memory;
    while (size--)
    {
        *byte++ = 0;
    }
}

static void
copy_memory(void* source, size_t size, void* dest)
{
    u8* u8_source = (u8*)source;
    u8* u8_dest = (u8*)dest;
    for (u64 i = 0; i < size; ++i)
    {
        u8_dest[i] = u8_source[i];
    }
}

static Memory_Arena
arena_init(void* memory, size_t size)
{
    Memory_Arena arena;
    arena.memory = memory;
    arena.size = size;
    arena.cursor = 0;
    arena.memory_left = size;

    return arena;
}

static void*
arena_push_size(Memory_Arena* arena, size_t size)
{
    Assert(arena->cursor + size <= arena->size);
    void* result = arena->memory + arena->cursor;
    arena->cursor += size;
    arena->memory_left -= size;

    return result;
}

static void
arena_pop_size(Memory_Arena* arena, size_t size)
{
    if (size > arena->cursor)
    {
        arena->cursor = 0;
        arena->memory_left = arena->size;
    }
    else
    {
        arena->cursor -= size;
        arena->memory_left += size;
    }
}

static macro void
arena_zero(Memory_Arena* arena)
{
    zero_memory(arena->memory, arena->size);
}

static macro void
arena_empty(Memory_Arena* arena)
{
    arena->cursor = 0;
    arena->memory_left = arena->size;
}

typedef struct String
{
    char* data;
    size_t size;
    u32 length;
}
String;

static u32
cstring_length(char* cstr)
{
    u32 size = 1;  // include null terminating character
    for (char* c = cstr; *c; ++c)  // NOTE: Count till *c is NULL
    {
        ++size;
    }

    return size;
}

static b32
cstring_compare(char* a, char* b)  // a == b ? 1 : 0, assumes null terminated strings
{
    char* ptr_a = a;
    char* ptr_b = b;
    b32 a_null;
    b32 b_null;
    while (*(ptr_a++) == *(ptr_b++))
    {
        a_null = *ptr_a == '\0';
        b_null = *ptr_b == '\0';
        if (a_null)
        {
            if(b_null)
            {
                return 1;
            }
            return 0;
        }
        else if (b_null)
        {
            if (a_null)
            {
                return 1;
            }
            return 0;
        }
    }

    return 0;
}

static b32
cstring_compare_fixedlengths(char* a, char* b, u32 length)
{
    for (int i = 0; i < length; ++i)
    {
        if (a[i] != b[i])
        {
            return 0;
        }
    }
    return 1;
}

static char*
cstring_cat(char* part1, char* part2)
{
    char* result = part1;
    while (*part1++);
    while (*part1++ = *part2++);
    return result;
}

static char*
arena_push_chars(Memory_Arena* arena, char* source)
{
    u32 size = cstring_length(source);

    char* dest = (char*)arena_push_size(arena, size);
    copy_memory(source, size, dest);

    return dest;
}

static String
arena_push_string(Memory_Arena* arena, char* format, ...)
{
    String string = { 0 };

    va_list args;
    va_start(args, format);
    u32 needed_bytes = vsnprintf(0, 0, format, args)+1;
    va_end(args);

    string.data = (char*)arena_push_size(arena, needed_bytes);
    if (string.data)
    {
        string.size = needed_bytes;
        string.length = needed_bytes-1;
        
        va_start(args, format);
        vsnprintf(string.data, needed_bytes, format, args);
        va_end(args);

        string.data[needed_bytes - 1] = '\0';
    }

    return string;
}

typedef struct Grouped_Array
{
    u32 array_count;
    u32* lengths;
    void** arrays;
}
Grouped_Array;

static Grouped_Array
arena_push_grouped_array(Memory_Arena* arena, u32 array_count, u32* lengths, size_t element_size)
{
    Grouped_Array grouping = { array_count, lengths, 0 };
    grouping.arrays = arena_push_size(arena, array_count * sizeof(void**));
    for (u32 i = 0; i < array_count; ++i)
    {
        grouping.arrays[i] = arena_push_size(arena, (size_t)lengths[i] * element_size);
    }
    
    return grouping;
}

#endif  // SK_MEMORY_ARENA_H
