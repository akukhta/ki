cmake_minimum_required(VERSION 3.0)
set(QT_CREATOR_SKIP_PACKAGE_MANAGER_SETUP ON)

project(IPCToolTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../CodeCoverage.cmake)
append_coverage_compiler_flags()


add_executable(${PROJECT_NAME}
        ${KI_SRC}
        main.cpp
)

target_link_libraries(${PROJECT_NAME} Boost::boost)

add_test(IPCToolTest ${PROJECT_NAME})