#!/bin/bash

cd ../libs
make

cd ../client
make
./client localhost 8081
make clean
