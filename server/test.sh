#!/bin/bash

# Script for testing server side of chit-cpp

# build
make

# test
if [ $# -eq 1 ]; then
	./main "$1"
else
	./main 8081
fi

# clean
make clean
