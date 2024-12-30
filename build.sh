#!/bin/bash
shopt -s expand_aliases

source $HOME/.bashrc

# set which cmake to use
while getopts :b: OPT; do
    case "$OPT" in
    b)
        export cmd="$OPTARG"
        ;;
    esac

done



cmake -S . -B build
threads=$((`nproc --all`/2 + 1))
echo "building project with thread:${threads}"
cmake --build build -j ${threads}
