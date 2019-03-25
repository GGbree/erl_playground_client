# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (erl_playground_client)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory !" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()


# Compile external dependencies 
add_subdirectory (external/plibsys)
add_subdirectory (external/protobuf/cmake)

include_directories(
	external/wxWidgets/include
	.
)

set(ALL_LIBS
    plibsys
	libprotobuf
)

add_executable(erl_playground_client
	src/main.cpp
	proto/erl_playground.pb.cc
	proto/erl_playground.pb.h
)

target_link_libraries(erl_playground_client
	${ALL_LIBS}
)