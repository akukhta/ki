cmake_minimum_required(VERSION 3.0)
set(QT_CREATOR_SKIP_PACKAGE_MANAGER_SETUP ON)

project(zeroSizeFileWrite)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Boost REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../CodeCoverage.cmake)
append_coverage_compiler_flags()

add_executable(${PROJECT_NAME}
    ${KI_SRC}
    main.cpp
)

target_link_libraries(${PROJECT_NAME} Boost::boost)
add_test(ZeroSizeFileWrite ${PROJECT_NAME})