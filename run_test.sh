#!/bin/bash

echo "building project..."
cmake -S . -B build 
cmake --build build
cur_dir=`pwd`
cd build/tests && ctest -VV
cd $cur_dir