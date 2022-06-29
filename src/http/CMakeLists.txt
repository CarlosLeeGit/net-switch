cmake_minimum_required(VERSION 3.8.0)

set(TARGET_NAME http)
set(HTTP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${HTTP_INCLUDE_DIR})
include_directories(${BASE_INCLUDE_DIR})

file(GLOB SRC_LIST *.cc)

# add_subdirectory(client)
include_directories(${HTTP_CLIENT_INC})
list(APPEND SRC_LIST ${HTTP_CLIENT_SRC})

add_subdirectory(server)
include_directories(${HTTP_SERVER_INC})
list(APPEND SRC_LIST ${HTTP_SERVER_SRC})

add_library(${TARGET_NAME} STATIC ${SRC_LIST})

set(HTTP_MODULE ${TARGET_NAME} PARENT_SCOPE)
set(HTTP_INCLUDE_DIR ${HTTP_INCLUDE_DIR} CACHE INTERNAL "http_include")

list(APPEND TEST_INCLUDE_DIR ${HTTP_INCLUDE_DIR})
set(TEST_INCLUDE_DIR ${TEST_INCLUDE_DIR} CACHE INTERNAL "test_include")