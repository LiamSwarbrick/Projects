@echo off

set game_exe=a

if "%1" == "prof" (
    set gcc_args=-g -pg
) else (
    set gcc_args=-g
)

gcc %gcc_args% external/*.c src/*.c -o %game_exe%.exe -Iexternal -Isrc/include -lopengl32 -lgdi32

@echo on
