#!/bin/bash
cd ./src/lib_xtools
if [ ! -d "build" ];then mkdir build
else
cd build
cmake ..
make 
make install
fi