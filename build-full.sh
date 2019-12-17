#!/usr/bin/env sh
#These next 4 lines get the absolute path to this script
pushd $(dirname "${0}") > /dev/null
repo_dir=$(pwd -L)
popd > /dev/null
build_dir="$repo_dir/build"
mkdir build
mkdir FL
echo "Build dir: $build_dir"
echo "Repository dir: $repo_dir"

cd fltk
./configure --libdir=$build_dir --bindir=$build_dir --includedir=$repo_dir --prefix=$build_dir
make install
cd ..

cp build/libfltk.a libfltk.a
# Don't care about any other build files besides libfltk.a
rm -r build
cd fltk
make clean
