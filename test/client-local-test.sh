#!/bin/bash

#cd ../libs
#make

#cd ../client
#make
#./main localhost 8081
#make clean

#cd  ../cmake-build-debug/src/client/
#./client localhost 8081
cd ..
rm -rf client-tmp
mkdir client-tmp
cmake ..
make
./client localhost 8081
