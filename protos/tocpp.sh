#!/bin/bash

protoc --proto_path=. --cpp_out=../src/lib_xprotos login.proto
