#!/bin/bash

#cd ../libs
#make

#cd ../server
#make
#./main 8081
#make clean

#cd ..
#mkdir tmp
#cd tmp
#cmake ..
#cd src/server

cd ../cmake-build-debug/src/server
./server 8081
