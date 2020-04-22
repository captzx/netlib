#!/bin/bash

protoc --proto_path=. --cpp_out=../src/protos login.proto
