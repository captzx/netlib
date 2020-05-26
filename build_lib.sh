#!/bin/bash

cd ./src/lib_xtools
if [ ! -d "build" ];then
       	mkdir build
else
	cd build
	cmake ..
	make
	make install
fi

cd ~/netlib
cd ./src/lib_xnet
if [ ! -d "build" ]; then
       	mkdir build
else
	cd build
	cmake ..
	make
	make install
fi

cd ~/netlib/src/lib_xprotos
if [ ! -d "build" ]; then
	mkdir build
else
	cd build
	cmake ..
	make
	make install
fi

cd ~/netlib/src/server_common
if [ ! -d "build" ]; then
	mkdir build
else
	cd build
	cmake ..
	make
	make install
fi


