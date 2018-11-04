#!/bin/bash

# -------------------------------------------------------
# variables being used for this script that can be changed
# -------------------------------------------------------
BUILD_DIR=chit-build
IP=localhost
PORT=8081

# -------------------------------------------------------
# switch to folder with everything built
# -------------------------------------------------------
cd "../../$BUILD_DIR"

# -------------------------------------------------------
# switch to dir that has client executable
# -------------------------------------------------------
cd src/client

# -------------------------------------------------------
# run client executable with hard coded vals
# -------------------------------------------------------
./client "$IP" "$PORT"
