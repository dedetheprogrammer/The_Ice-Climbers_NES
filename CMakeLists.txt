cmake_minimum_required(VERSION 3.0)
project(ice_climbers)

set(CMAKE_GENERATOR "MinGW Makefiles")
set(CMAKE_CXX_STANDARD 17)

# Configurar la ruta de inclusión y las opciones de compilación
include_directories(
    lib/engine
    lib/raylib
)

# Configurar la ruta de las bibliotecas y enlazarlas
link_directories(
    lib/engine
    lib/raylib
)

add_executable(main maintest.cpp)

target_link_libraries(main
    engine
    raylib
    opengl32
    gdi32
    winmm
)

# Configurar opciones de compilación
target_compile_options(main PRIVATE
    -Wall
    -Wno-missing-braces
)
