#!/bin/bash

echo "building project..."
cmake -S . -B build 
cmake --build build
cur_dir=`pwd`
flag=""

while getopts :V OPT; do 
    case "$OPT" in 
        V) flag="-VV"
            echo "verbose mode"
            ;;
        \?) echo "unknown flag"
            exit -1
            ;;
    esac
done

cmd="ctest $flag"

cd build/tests && $cmd 
cd $cur_dir
