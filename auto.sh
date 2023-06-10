#!/bin/bash
LIBRARIES=Libraries
ENGINE=${LIBRARIES}/engine
RAYLIB=${LIBRARIES}/raylib
SCRIPTS=Assets/Scripts

g++ -c ${ENGINE}/Dengine.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/Dengine.o
g++ -c ${ENGINE}/DengineUI.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/DengineUI.o
g++ -c ${ENGINE}/raylibx.cpp -Wall -Wno-missing-braces -std=c++17 -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${ENGINE}/raylibx.o

ar rvs ${ENGINE}/libengine.a ${ENGINE}/Dengine.o ${ENGINE}/DengineUI.o ${ENGINE}/raylibx.o
rm ${ENGINE}/Dengine.o ${ENGINE}/DengineUI.o ${ENGINE}/raylibx.o

g++ -c ${SCRIPTS}/Controllers.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${SCRIPTS}/Controllers.o
g++ -c ${SCRIPTS}/Cloud.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${SCRIPTS}/Cloud.o
g++ -c ${SCRIPTS}/Player.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm -o ${SCRIPTS}/Player.o

echo iceclimber_icon ICON "iceclimber.ico" > iceclimber.rc
windres iceclimber.rc iceclimber_icon.o
g++ main.cpp -Wall -Wno-missing-braces -std=c++17 -I${SCRIPTS} -I${ENGINE} -L${ENGINE} -lengine -I${RAYLIB} -L${RAYLIB} -lraylib -lopengl32 -lgdi32 -lwinmm \
    ${SCRIPTS}/Controllers.o ${SCRIPTS}/Cloud.o ${SCRIPTS}/Player.o iceclimber_icon.o -o iceclimber
rm ${SCRIPTS}/Controllers.o ${SCRIPTS}/Cloud.o ${SCRIPTS}/Player.o iceclimber_icon.o iceclimber.rc
./iceclimber
