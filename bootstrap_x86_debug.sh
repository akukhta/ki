#!/bin/sh

conan install . --build=missing --output-folder=debug --profile=debugprofile.txt --profile:b=debugprofile.txt

