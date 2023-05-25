#!/bin/bash
LIBRARIES=Libraries
ENGINE=${LIBRARIES}/engine
RAYLIB=${LIBRARIES}/raylib
SCRIPTS=Assets/Scripts

g++ -c ${ENGINE}/EngineECS.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/EngineECS.o
g++ -c ${ENGINE}/EngineUI.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/EngineUI.o
g++ -c ${ENGINE}/raylibx.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/raylibx.o

ar rvs ${ENGINE}/libengine.a ${ENGINE}/EngineECS.o ${ENGINE}/EngineUI.o
rm ${ENGINE}/EngineECS.o ${ENGINE}/EngineUI.o ${ENGINE}/raylibx.o 

g++ -c ${SCRIPTS}/Controllers.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${SCRIPTS}/Controllers.o
g++ -c ${SCRIPTS}/Cloud.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${SCRIPTS}/Cloud.o
g++ -c ${SCRIPTS}/Player.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${SCRIPTS}/Player.o


g++ main.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm \
    ${SCRIPTS}/Controllers.o ${SCRIPTS}/Cloud.o ${SCRIPTS}/Player.o -o main && ./main
rm ${SCRIPTS}/Controllers.o ${SCRIPTS}/Cloud.o ${SCRIPTS}/Player.o 
