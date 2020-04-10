#!/bin/bash

NCORES=4
unamestr=`uname`
if [[ "$unamestr" == "Linux" ]]; then
        NCORES=`grep -c ^processor /proc/cpuinfo`
fi

if [[ "$unamestr" == "Darwin" ]]; then
        NCORES=`sysctl -n hw.ncpu`
fi

cd extern/maxmatch_MV/
make -j $NCORES
cd ../../

rm -rf deploy
rm -rf build
mkdir build
cd build
cmake ../
make -j $NCORES
cd ..

mkdir deploy
cp ./build/dynmatch deploy/
cp ./build/conver* deploy/
cp ./build/clean* deploy/
rm -rf build

