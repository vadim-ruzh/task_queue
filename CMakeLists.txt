cmake_minimum_required(VERSION 3.22)

project(task_queue VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(${CMAKE_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_SOURCE_DIR}/context)

add_test(Tests COMMAND QueueTests)