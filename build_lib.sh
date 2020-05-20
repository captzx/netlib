#!/bin/bash

cd ./src/lib_xtools
if [ ! -d "build" ];then
       	mkdir build
else
	cd build
	cmake ..
	make -j6 
	make install
fi

cd ~/netlib
cd ./src/lib_xnet
if [ ! -d "build" ]; then
       	mkdir build
else
	cd build
	cmake ..
	make -j6
	make install
fi

cd ~/netlib/src/lib_xprotos
if [ ! -d "build" ]; then
	mkdir build
else
	cd build
	cmake ..
	make -j6
	make install
fi


