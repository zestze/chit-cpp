#!/bin/bash

cd ../libs
make

cd ../server
make
./main 8081
make clean
