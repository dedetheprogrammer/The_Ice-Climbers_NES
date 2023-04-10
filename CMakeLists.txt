# Hello fellas, today I'll show you how this works:
#   1. Generate the Makefile:
#       cmake -G "MinGW Makefiles" -DBUILD_ENGINE=ON -DBUILD_GAME=ON
#   2. Build the project:
#       mingw32-make
#   3. Execute the project:
#       ./main
#   4. Or you can use the script:
#       sh ./build.sh
#   5. If you don't have cmake because you're not a weirdo like me, use auto.sh:
#       sh ./auto.sh

cmake_minimum_required(VERSION 3.0)
project(ice_climbers)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_ENGINE "Build engine" ON)
option(BUILD_GAME "Build game" ON)

if(BUILD_ENGINE)
    add_library(engine STATIC libraries/engine/EngineECS.cpp libraries/engine/raylibx.cpp)
    target_compile_options(engine PRIVATE -Wall -Wno-missing-braces)
    target_include_directories(engine PRIVATE libraries/raylib)
    target_link_libraries(engine PRIVATE ${CMAKE_SOURCE_DIR}/libraries/raylib/libraylib.a opengl32 gdi32 winmm)
    set_target_properties(engine PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/engine/)
endif()

if(BUILD_GAME)
    # Configurar la ruta de inclusión y las opciones de compilación
    include_directories(
        libraries/engine
        libraries/raylib
    )

    # Configurar la ruta de las bibliotecas y enlazarlas
    link_directories(
        libraries/engine
        libraries/raylib
    )

    add_executable(main main.cpp)

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
endif()