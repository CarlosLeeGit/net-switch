cmake_minimum_required(VERSION 3.8.0)

set(DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/download)
set(DOWNLOAD_BUILD_DIR ${DOWNLOAD_DIR}/build)

file(MAKE_DIRECTORY ${DOWNLOAD_BUILD_DIR})

set(GOOGLETEST_REPO "https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip")
set(GOOGLETEST_SOURCE_DIR ${DOWNLOAD_DIR}/googletest)

configure_file(./download/CMakeLists.txt.in ${DOWNLOAD_DIR}/CMakeLists.txt)

execute_process(COMMAND cmake ../ COMMAND make WORKING_DIRECTORY ${DOWNLOAD_BUILD_DIR})

add_subdirectory(${GOOGLETEST_SOURCE_DIR} ${DOWNLOAD_BUILD_DIR})