
#!/bin/sh

conan install . --output-folder=bin 
cmake . -B bin --preset conan-release
cd bin
make
cd ..
