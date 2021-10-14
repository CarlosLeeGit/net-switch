cmake_minimum_required(VERSION 3.8.0)

set(TARGET_NAME base)
set(BASE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(./include)

file(GLOB_RECURSE SRC_LIST *.cc)

add_library(${TARGET_NAME} STATIC ${SRC_LIST})

target_link_libraries(${TARGET_NAME} ssl crypto)

set(BASE_MODULE ${TARGET_NAME} PARENT_SCOPE)
set(BASE_INCLUDE_DIR ${BASE_INCLUDE_DIR} CACHE INTERNAL "base_include")

list(APPEND TEST_INCLUDE_DIR ${BASE_INCLUDE_DIR})
set(TEST_INCLUDE_DIR ${TEST_INCLUDE_DIR} CACHE INTERNAL "test_include")