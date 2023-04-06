#!/bin/bash
g++ -c raylibx.cpp EngineECS.cpp -Wall -Wno-missing-braces -std=c++17 -I./lib/raylib -L./lib/raylib -lraylib -lopengl32 -lgdi32 -lwinmm
ar rvs libengine.a EngineECS.o raylibx.o
rm EngineECS.o raylibx.o
