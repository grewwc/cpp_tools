#!/bin/bash

echo "building project..."
cmake -S . -B build 
cmake --build build
cur_dir=`pwd`
flag=""

while getopts :V OPT; do 
    case "$OPT" in 
        V) flag="-V"
            ;;
        \?) echo "unknown flag"
            exit -1
            ;;
    esac
done

cmd="ctest $flag --gtest_filter=STRING_TEST.string_plus"

cd build/tests && $cmd 
cd $cur_dir