#!/bin/bash

# Script for testing server side of chit-cpp

# build
make

# test
if [ $# -eq 1 ]; then
	./server "$1"
else
	./server 8081
fi

# clean
make clean
