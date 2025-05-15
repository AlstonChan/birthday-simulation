cmake_minimum_required(VERSION 3.10)
project(birthday_simulation C)

set(CMAKE_C_STANDARD 17)

# Recursively gather all source files in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)

# Define executable and output dir
add_executable(birthday_simulation ${SOURCES})

# Link with libraries
target_link_libraries(birthday_simulation PRIVATE menuw ncursesw)

# Custom output directory (like your Makefile)
set_target_properties(birthday_simulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
