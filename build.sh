#!/bin/bash

cmd=$(which cmake3)

if [[ $? -ne 0 ]]; then
    cmd=$(which cmake)
fi


while getopts :b:g: OPT; do
    case "$OPT" in
    b)
        export cmd="$OPTARG"
        ;;
    g)
        export build_grpc=true
        ;;
    esac

done

echo "building project..."

$cmd -S . -B build
# $cmd --build build -j 6
