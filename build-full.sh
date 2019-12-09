#!/usr/bin/env sh
repo_dir=`pwd`
build_dir="$repo_dir/build"
mkdir build
mkdir FL
include_dir="$repo_dir"
echo "Build dir: $build_dir"
echo "Include dir: $include_dir"

cd fltk
./configure --libdir=$build_dir --bindir=$build_dir --includedir=$include_dir --prefix=$build_dir
make install
cd ..

cp build/libfltk.a libfltk.a
cd fltk
make clean
