#!/bin/bash

# Script for testing client side of chit-cpp

# build
make

# test
if [ $# -eq 2 ]; then
	./client "$1" "$2"
else
	./client localhost 8080
fi

# clean
make clean
