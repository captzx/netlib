#!/bin/bash

protoc --proto_path=. --cpp_out=../src/protos/cpp login.proto
