#!/bin/bash

cmd=`which cmake`

while getopts :b: OPT; do 
    case "$OPT" in 
        b) cmd="$OPTARG"
            ;;
    esac

done 

echo "here ==> $cmd"

echo "building project..."

$cmd -S . -B build 
$cmd --build build -j 4
