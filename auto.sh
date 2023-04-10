#!/bin/bash
LIBRARIES=libraries
ENGINE=${LIBRARIES}/engine
RAYLIB=${LIBRARIES}/raylib

g++ -c ${ENGINE}/EngineECS.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/EngineECS.o
g++ -c ${ENGINE}/raylibx.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/raylibx.o
ar rvs ${ENGINE}/libengine.a ${ENGINE}/EngineECS.o ${ENGINE}/raylibx.o
rm ${ENGINE}/EngineECS.o ${ENGINE}/raylibx.o
g++ main.cpp -Wall -Wno-missing-braces -std=c++17 -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o main && ./main