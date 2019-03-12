#!/bin/bash

# -------------------------------------------------------
# this is the directory we plan on building everything to
# -------------------------------------------------------
BUILD_DIR=chit-build
cd ../..
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
fi
mkdir "$BUILD_DIR" && cd "$BUILD_DIR"

# -------------------------------------------------------
# this needs to be set to a c++17 capable compiler
# -------------------------------------------------------
#C_COMPILER=/usr/bin/clang-6.0
#CXX_COMPILER=/usr/bin/clang++-6.0
C_COMPILER=/usr/bin/clang-7
CXX_COMPILER=/usr/bin/clang++-7

# -------------------------------------------------------
# this will set the appropriate compile flags 
# -------------------------------------------------------
CMAKE_FLAGS=Release
#CMAKE_FLAGS=Debug

# -------------------------------------------------------
# invoke cmake to setup build files
# pre set to using makefiles for the generation
# -------------------------------------------------------
CC="$C_COMPILER" CXX="$CXX_COMPILER" cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$CMAKE_FLAGS"

# -------------------------------------------------------
# now, build
# -------------------------------------------------------
make
