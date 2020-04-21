#!/bin/bash


find . -name "*.h" -o -name "*.cpp" > cscope.file

cscope -bkq -i cscope.file

ctags -R
