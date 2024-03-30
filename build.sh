#!/bin/bash
shopt -s expand_aliases

source $HOME/.bashrc

while getopts :b: OPT; do
    case "$OPT" in
    b)
        export cmd="$OPTARG"
        ;;
    esac

done

echo "building project..."


cmake -S . -B build
# $cmd --build build -j 6
