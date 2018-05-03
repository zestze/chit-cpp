#!/bin/bash

# Script for testing client side of chit-cpp

# build
make

# test
if [ $# -eq 2 ]; then
	./main "$1" "$2"
else
	./main localhost 8081
fi

# clean
make clean
