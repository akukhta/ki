cmake_minimum_required(VERSION 3.0)
project(kiTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

file(GLOB KI_SRC
        "../src/common/*.h"
        "../src/common/*.hpp"
        "../src/common/*.cpp"
        "../src/common/*.c"
        "../src/IPC/*.h"
        "../src/IPC/*.hpp"
        "../src/IPC/*.cpp"
        "../src/IPC/*.c"
        "../src/queue/*.h"
        "../src/queue/*.hpp"
        "../src/queue/*.cpp"
        "../src/queue/*.c"
        "../src/reader/*.h"
        "../src/reader/*.hpp"
        "../src/reader/*.cpp"
        "../src/reader/*.c"
        "../src/writer/*.h"
        "../src/writer/*.hpp"
        "../src/writer/*.cpp"
        "../src/writer/*.c"
)

message("SOURCES:${KI_SRC}")

enable_testing()

add_subdirectory(reader/MMapReaderTest)
add_subdirectory(writer/MMapWriterTest)
add_subdirectory(queue/SyncQueue)
add_subdirectory(copyTool/parallelCopyTool)

#include(${CMAKE_CURRENT_SOURCE_DIR}/CodeCoverage.cmake)
#append_coverage_compiler_flags()

set(COVERAGE_MAIN "coverage")

set(COVERAGE_EXCLUDES
        "${BOOST_LIBRARY_DIR}/*"
        "${PROJECT_SOURCE_DIR}/test/*"
        "/usr/include/*"
        )

set(COVERAGE_EXTRA_FLAGS)

set(COVERAGE_DEPENDENCIES "plainTest")

setup_target_for_coverage_gcovr_html(
        NAME
        ${COVERAGE_MAIN}
        EXECUTABLE
        ctest
        DEPENDENCIES
        ${COVERAGE_DEPENDENCIES}
        BASE_DIRECTORY
        ${CMAKE_SR})
