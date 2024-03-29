# ki 
+ [![cov](https://akukhta.github.io/ki/badges/coverage.svg)](https://github.com/akukhta/ki/actions)

Materials
Before you start (or after) It is recommended to go through everything what modern C++ offers in multithreading here https://en.cppreference.com/w/cpp/thread

modern multithreading
Practical Task Description
Your task is to implement simple 2 threaded "copy" tool.

Tool should be implemented as a console application.
Tool should accept 2 parameters source filename and target filename.
Copying logic should be organized with help of 2 threads.
First thread should read the data from source file.
Second thread should write the data to the target file.
 

 

Hints before start:
* Start the project as a prove of concept.
* no need to process all possible errors. Just check the count of parameters so 'source' and 'destination' parameters are present and source exists. (start with POC - prove of concept)
* safety hint - do not overwrite destination file by default - but it is up to you.
* If you enjoy process or wants to continue: Create complete functional tool for backup process. Just pick up technical requirements for your tool from "copy /?" (windows) or "cp -man" (linux)

![alt text](diagram.png "Title")
