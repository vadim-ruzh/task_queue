cmake_minimum_required(VERSION 3.23)
project(task_queue)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB Context.cpp  Context.hpp)

add_library(context STATIC ${SOURCE_LIB})

add_executable(main ${SOURCE_EXE})

set_target_properties(main PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED OFF
        COMPILE_OPTIONS "-fsanitize=thread"
        LINK_OPTIONS "-fsanitize=thread"
        )

target_link_libraries(context
        ${CMAKE_THREAD_LIBS_INIT}
        )

target_link_libraries(main context)