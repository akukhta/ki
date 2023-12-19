cmake_minimum_required(VERSION 3.0)
project(kiTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

add_subdirectory(reader/MMapReaderTest)
add_subdirectory(writer/MMapWriterTest)
add_subdirectory(queue/SyncQueue)
add_subdirectory(copyTool/parallelCopyTool)