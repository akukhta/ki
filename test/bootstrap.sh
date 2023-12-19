#!/bin/sh

conan install . --output-folder=bin 
cmake . -B bin --preset conan-release
cmake --build bin
cd bin
ctest
