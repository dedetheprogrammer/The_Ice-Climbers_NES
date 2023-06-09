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
    add_library(engine STATIC Libraries/engine/Dengine.cpp Libraries/engine/DengineUI.cpp Libraries/engine/raylibx.cpp)
    target_compile_options(engine PRIVATE -Wall -Wno-missing-braces)
    target_include_directories(engine PRIVATE Libraries/raylib)
    target_link_libraries(engine PRIVATE ${CMAKE_SOURCE_DIR}/Libraries/raylib/libraylib.a opengl32 gdi32 winmm)
    set_target_properties(engine PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Libraries/engine/)
endif()

if(BUILD_GAME)
    # Configurar la ruta de inclusión y las opciones de compilación
    include_directories(
        Assets/Scripts
        Libraries/engine
        Libraries/raylib
    )

    # Configurar la ruta de las bibliotecas y enlazarlas
    link_directories(
        Libraries/engine
        Libraries/raylib
    )

    add_custom_command(
        OUTPUT iceclimber.rc
        COMMAND echo iceclimber_icon ICON "iceclimber.ico" > iceclimber.rc
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS iceclimber.ico
    )

    add_executable(iceclimber main.cpp)

    target_link_libraries(
        iceclimber
        engine
        raylib
        opengl32
        gdi32
        winmm
    )

    target_sources(iceclimber PRIVATE
        Assets/Scripts/Player.cpp
        Assets/Scripts/Controllers.cpp
        Assets/Scripts/Cloud.cpp
        iceclimber.rc
    )

    # Configurar opciones de compilación
    target_compile_options(iceclimber PRIVATE
        -Wall
        -Wno-missing-braces
    )

endif()