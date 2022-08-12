set(SOURCE_LIB Context.cpp Context.hpp)

find_package(Threads)

add_library(context STATIC ${SOURCE_LIB})

target_link_libraries(context
	${CMAKE_THREAD_LIBS_INIT}
	)
