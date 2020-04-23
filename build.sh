#!/bin/bash

cd ~/netlib
if [ ! -d "build" ]; then
	make build
else
	cd build
	cmake ..
	make
fi

