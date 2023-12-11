
#!/bin/sh

conan install . --output-folder=bin 
cmake . -B bin
cd bin
make
cd ..
