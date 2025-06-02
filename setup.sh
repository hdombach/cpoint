#! /bin/bash

BUILD_TYPE=Debug

if [ $# -gt 1 ]; then
	echo "Need to provide built type"
	echo "$0 <build type>"
	exit 1
elif [ $# -eq 1 ]; then
BUILD_TYPE="$1"
fi

mkdir build
CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja"

if [ "$BUILD_TYPE" = "Debug" ]; then
	CMAKE_FLAGS"$CMAKE_FLAGS -O0"
else
	CMAKE_FLAGS"$CMAKE_FLAGS -O2"

fi

if [ -n "$CPOINT_CC" ]; then
	CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_C_COMPILER=$CPOINT_CC"
fi

if [ -n "$CPOINT_CXX" ]; then
	CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_CXX_COMPILER=$CPOINT_CXX"
fi


cmake -B build $CMAKE_FLAGS
