#!/bin/bash

cd ../libs
make

cd ../client
make
./main localhost 8081
make clean
