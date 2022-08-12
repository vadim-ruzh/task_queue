
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests test.cpp)

include_directories(${CMAKE_SOURCE_DIR}/context)
target_link_libraries(tests gtest_main context)