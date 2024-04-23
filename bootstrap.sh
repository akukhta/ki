#!/bin/sh

conan install . --output-folder=bin --profile=debugprofile.txt --profile:b=debugprofile.txt --build=missing
cmake . -B bin -DCMAKE_BUILD_TYPE=Debug --preset conan-debug
cmake --build bin