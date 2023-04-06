#!/bin/bash
g++ maintest.cpp -Wall -Wno-missing-braces -std=c++17 -I./lib/engine -L./lib/engine -lengine -I./lib/raylib -L./lib/raylib -lraylib -lopengl32 -lgdi32 -lwinmm -o main && ./main