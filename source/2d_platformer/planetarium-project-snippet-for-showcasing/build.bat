@echo off

gcc -g src/*.c src/components/*.c .external/*.c -o planet.exe -I.external/include -L.external/lib -lraylib -lopengl32 -lgdi32 -lwinmm

@echo on
