#!/bin/bash
cmake -G "MinGW Makefiles" -DBUILD_ENGINE=ON -DBUILD_GAME=ON #-B build
mingw32-make
./iceclimber