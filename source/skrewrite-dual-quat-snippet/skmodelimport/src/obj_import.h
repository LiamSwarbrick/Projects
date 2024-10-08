#ifndef OBJ_IMPORT_H
#define OBJ_IMPORT_H

// TOMORROW: https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_020_Skins.md

typedef struct Static_Vertex
{
    Vector3 p;  // position
    Vector2 t;  // texture
    Vector3 n;  // normal
}
Static_Vertex;

int
obj_import_main(int argc, char** argv)
{
    if (argc <= 1)
    {
        printf("No command line arguments passed. Pass wavefront obj file as parameter e.g \"example.obj\".\n");
        return -1;
    }



    return 0;
}

#endif  // OBJ_IMPORT_H
