cmake_minimum_required(VERSION 3.0)
set(QT_CREATOR_SKIP_PACKAGE_MANAGER_SETUP ON)

project(emptyQueue)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME} 
    ../../../../src/queue/SynchronizedQueue.hpp
    ../../../../src/queue/SynchronizedQueue.cpp
    ../../../Utiles/TmpFile.hpp
    main.cpp
)

target_link_libraries(${PROJECT_NAME} Boost::boost)

add_test(EmptyQueue ${PROJECT_NAME})