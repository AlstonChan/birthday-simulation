cmake_minimum_required(VERSION 3.10)
project(birthday_simulation C)

set(CMAKE_C_STANDARD 17)

# Recursively gather all source files in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)

# Define executable and output dir
add_executable(birthday_simulation ${SOURCES})

# Tell CMake to add the 'include' directory to the compiler's include search path
# PRIVATE means that only this target (birthday_simulation) needs these includes.
target_include_directories(birthday_simulation PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link with libraries
target_link_libraries(birthday_simulation PRIVATE menuw ncursesw formw ssl crypto)

# Custom output directory (like your Makefile)
set_target_properties(birthday_simulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
