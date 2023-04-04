#!/bin/bash
g++ maintest.cpp EngineECS.cpp raylibx.cpp -Wall -Wno-missing-braces -std=c++17 -I../raylib -L../raylib -lraylib -lopengl32 -lgdi32 -lwinmm -o main && ./main