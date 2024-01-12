#!/bin/sh

conan install . --build=missing --output-folder=release --profile=releaseprofile.txt --profile:b=releaseprofile.txt

