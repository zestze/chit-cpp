#!/bin/bash

serv_ip=159.203.173.50

cd ../libs
make

cd ../client
make
./main $serv_ip 8081
make clean
