cmake_minimum_required(VERSION 3.0)
set(QT_CREATOR_SKIP_PACKAGE_MANAGER_SETUP ON)

project(plainTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME} 
    ../../../../src/common/ParallelCopyTool.hpp
    ../../../../src/common/ParallelCopyTool.cpp
    ../../../../src/common/IOptionsParser.hpp
    ../../../../src/reader/MMapFileReader.hpp
    ../../../../src/reader/MMapFileReader.cpp
    ../../../../src/writer/MMapFileWriter.hpp
    ../../../../src/writer/MMapFileWriter.cpp
    ../../../../src/queue/SynchronizedQueue.cpp
    ../../../../src/common/PseudoToolFactory.hpp
    ../../../Utiles/TmpFile.hpp
    ../../../Utiles/MockedOptionsParser.hpp
    main.cpp
)

target_link_libraries(${PROJECT_NAME} Boost::boost)

add_test(PlainTest ${PROJECT_NAME})