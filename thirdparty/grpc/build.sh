#!/bin/bash
echo "here " $build_grpc
if [[ "$build_grpc" != "true" ]]; then 
    exit 0
fi

name=libgrpc_all.a

function archive() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "Mac"
        libtool -static -o $name *.a
    elif [[ "$OSTYPE" == "linux-gnu" ]]; then
        echo "Linux"
        for f in $(ls | grep '\.a'); do
            ar -x "$f"
        done
        ar -rc $name *.o
    fi
}

script_dir=$(dirname "$BASH_SOURCE")
script_dir=`realpath ${script_dir}`

if [ ! -d "$script_dir/lib" ]; then 
    mkdir -p "$script_dir/lib"
fi

pushd $script_dir


if [ ! -d cmake/build ]; then
    mkdir -p cmake/build
fi

pushd cmake/build

cmd=$(which cmake3)

if [[ $? -ne 0 ]]; then
    cmd=$(which cmake)
fi

${cmd} -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX="${script_dir}" \
    ../..

make -j 6
make install

popd

# in $script_dir
pushd lib
archive 
mv $name ../../../lib/
popd

popd
