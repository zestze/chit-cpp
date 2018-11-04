#!/bin/bash

# -------------------------------------------------------
# variables being used for this script that can be changed
# -------------------------------------------------------
BUILD_DIR=chit-build
PORT=8081

# -------------------------------------------------------
# switch to folder with everything built
# -------------------------------------------------------
cd "../../$BUILD_DIR"

# -------------------------------------------------------
# switch to dir that has server executable
# -------------------------------------------------------
cd src/server

# -------------------------------------------------------
# run server executable with hard coded vals
# -------------------------------------------------------
./server "$PORT"
