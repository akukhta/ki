cmake_minimum_required(VERSION 3.0)

project(ki)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Boost 1.82.0 REQUIRED)

add_executable(${PROJECT_NAME} 
	src/common/main.cpp
	src/common/ICopyTool.hpp
	src/common/ParallelCopyTool.hpp
	src/common/ParallelCopyTool.cpp
	src/common/OptionsParser.hpp
	src/queue/IQueue.hpp
	src/queue/SynchronizedQueue.hpp
	src/queue/SynchronizedQueue.cpp
	src/reader/IFileReader.hpp
	src/reader/MMapFileReader.hpp
	src/reader/MMapFileReader.cpp
	src/writer/IFileWriter.hpp
	src/writer/MMapFileWriter.hpp
	src/writer/MMapFileWriter.cpp
)

target_link_libraries(${PROJECT_NAME} Boost::program_options)