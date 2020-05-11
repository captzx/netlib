#!/bin/bash

cd ~/netlib
if [ ! -d "build" ]; then
	mkdir build
else
	cd build
	cmake ..
	make
fi

