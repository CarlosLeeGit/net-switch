cmake_minimum_required(VERSION 3.8.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(TARGET_NAME net-switch)

add_subdirectory(base)
add_subdirectory(http)
# add_subdirectory(proxy)

set(SRC ${CMAKE_CURRENT_BINARY_DIR}/empty.cc)
file(WRITE ${SRC} "")
add_library(${TARGET_NAME} SHARED ${SRC})

target_link_libraries(${TARGET_NAME} ${HTTP_MODULE})
target_link_libraries(${TARGET_NAME} ${BASE_MODULE})
target_link_libraries(${TARGET_NAME} pthread)

set(NET_SWITCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "net_switch_include")