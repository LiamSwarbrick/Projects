#include "memory_arena.h"
#include "obj_import.h"
#include "gltf_import.h"

int
main(int argc, char** argv)
{
    /*u32 filename_length = cstring_length(argv[1]+1); // idk why +1
    if (argv[1][filename_length - 3] == 'o' &&
        argv[1][filename_length - 2] == 'b' &&
        argv[1][filename_length - 1] == 'j')
    {
        return obj_import_main(argc, argv);
    }*/
    if (argv[1][filename_length - 4] == 'g' &&
        argv[1][filename_length - 3] == 'l' &&
        argv[1][filename_length - 2] == 't' &&
        argv[1][filename_length - 1] == 'f')
    {
        return gltf_import_main(argc, argv);
    }
}
