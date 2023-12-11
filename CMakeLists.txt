cmake_minimum_required(VERSION 3.0)
set(QT_CREATOR_SKIP_PACKAGE_MANAGER_SETUP ON)

project(ki)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(${PROJECT_NAME} 
	src/common/main.cpp
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
