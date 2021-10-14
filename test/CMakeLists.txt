cmake_minimum_required(VERSION 3.8.0)

set(TARGET_NAME net-switch-test)

file(GLOB_RECURSE SRC_LIST *.cc)

include_directories(${TEST_INCLUDE_DIR})

add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL ${SRC_LIST})

target_link_libraries(${TARGET_NAME} gtest)
target_link_libraries(${TARGET_NAME} net-switch)