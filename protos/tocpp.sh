#!/bin/bash

protoc --proto_path=. --cpp_out=../src/lib_xprotos Server.proto

protoc --proto_path=. --cpp_out=../src/lib_xprotos Login.proto
