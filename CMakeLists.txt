#https://cmake.org/cmake/help/latest/guide/tutorial/index.html
cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_C_COMPILER clang)
project (
	mangle
	LANGUAGES C
)
#set(CMAKE_C_FLAGS_DEBUG_INIT "-g -Wall -fsanitize=undefined -fno-sanitize-recover=undefined")
#set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -Wall ")
add_compile_options(
  -Wall
  $<$<CONFIG:DEBUG>:-g>
  $<$<CONFIG:DEBUG>:-fsanitize=undefined>
  $<$<CONFIG:DEBUG>:-fno-sanitize-recover=undefined>
  $<$<CONFIG:RELEASE>:-O3>
)
add_link_options(
	$<$<CONFIG:DEBUG>:-fsanitize=undefined>
	$<$<CONFIG:DEBUG>:-fno-sanitize-recover=undefined>
)
add_compile_definitions(
  $<$<CONFIG:DEBUG>:DEBUG>
)
add_library(cppmangle SHARED
	allocator.c
	demangle.c
	mangle.c
	substitutions.c
	cpptypes.c
	interface.c
)
add_executable(
	mangling_unit_tests
	mangling_unit_tests.c
)

target_link_libraries(mangling_unit_tests PUBLIC cppmangle check )
