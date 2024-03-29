#!/bin/sh

conan install . --output-folder=bin --profile=../debugprofile.txt --profile:b=../debugprofile.txt --build=missing
cmake . -B bin -DCMAKE_BUILD_TYPE=Debug --preset conan-debug
cmake --build bin --config debug
cd bin
ctest

cd ../
mkdir coverage
cd ..

gcovr --gcov-executable gcov-13 . --json-summary --json ./test/coverage/coverage-summary.json --json-summary-pretty --cobertura ./test/coverage/coverage.cobertura.xml --cobertura-pretty
lcov -c -d . -o ./test/coverage/my.info --gcov-tool gcov-13 --exclude "*/boost/*" --exclude "13/*" --exclude "/usr/include/*" --exclude "*/test/*"
genhtml -o ./test/coverage/ ./test/coverage/my.info

cd test