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
	src/common/OptionsParser.hpp
	src/queue/IQueue.hpp
	src/queue/SynchronizedQueue.hpp
	src/queue/BufferedQueue.hpp
	src/queue/Buffer.hpp
	src/reader/IFileReader.hpp
	src/reader/MMapFileReader.hpp
	src/writer/IFileWriter.hpp
	src/writer/MMapFileWriter.hpp
)

target_link_libraries(${PROJECT_NAME} Boost::program_options)