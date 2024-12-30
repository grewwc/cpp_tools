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

echo "building project..."


cmake -S . -B build
threads=$((`nproc -all`/2 + 1))
cmake --build build -j ${threads}
