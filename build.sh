#!/bin/bash

cmd=$(which cmake3)

if [[ $? -ne 0 ]]; then
    cmd=$(which cmake)
fi

while getopts :b: OPT; do
    case "$OPT" in
    b)
        cmd="$OPTARG"
        ;;
    esac

done

echo "building project..."

$cmd -S . -B build
$cmd --build build -j 4
