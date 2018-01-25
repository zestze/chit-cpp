#!/bin/bash

cd ../libs
make

cd ../server
make
./server 8081
make clean
