cmake_minimum_required(VERSION 3.19)

project(Birthday_Simulation LANGUAGES C VERSION 1.0.1)
message("CMAKE_PROJECT_VERSION = ${CMAKE_PROJECT_VERSION}")

add_compile_definitions(
    PROJECT_VERSION_MAJOR=${Birthday_Simulation_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${Birthday_Simulation_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${Birthday_Simulation_VERSION_PATCH}
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Explicitly set to prefer static libraries.
# This affects find_package and general linking behavior.
option(USE_STATIC_LIBS "Use static linking for dependencies" ON)

if(WIN32)
    # This block runs on MSYS2/MinGW (which CMake detects as WIN32)
    message(STATUS "Configuring for WIN32 (MSYS2/MinGW)")
    
    # Set Windows-specific linker flags for static build
    if(USE_STATIC_LIBS)
        # These aggressive flags are often needed on MinGW for static linking
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -Bstatic -Wl,-Bdynamic")
    endif()

    # Set Windows-specific prefix paths for finding packages
    set(CMAKE_PREFIX_PATH
        "C:/msys64/ucrt64"
        "/ucrt64"
        "/usr/local"
    )
else()
    # This block runs on Linux/Debian
    message(STATUS "Configuring for POSIX (Linux/Debian)")
    
    # Use standard POSIX/Linux paths
    set(CMAKE_PREFIX_PATH
        "/usr/local"
    )
endif()

if(USE_STATIC_LIBS)
    set(BUILD_SHARED_LIBS OFF)
    add_compile_definitions(NCURSES_STATIC)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(OPENSSL_USE_STATIC_LIBS TRUE)
else()
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".dll" ".dylib")
endif()


# Embed the markdown file content
include("${CMAKE_SOURCE_DIR}/EmbedMarkdown.Cmake" REQUIRED)
message(STATUS "EmbedMarkdown.cmake included")

set(MARKDOWN_FILE "${CMAKE_SOURCE_DIR}/src/explanation.md")
set(GENERATED_C "${CMAKE_BINARY_DIR}/explanation_embedded.c")
set(GENERATED_H "${CMAKE_BINARY_DIR}/explanation_embedded.h")

embed_markdown("${MARKDOWN_FILE}" "${GENERATED_C}" "${GENERATED_H}" "explanation_embedded")
configure_file(
    ${CMAKE_SOURCE_DIR}/src/explanation.md
    ${CMAKE_BINARY_DIR}/explanation.md
    COPYONLY
)

# Recursively gather all source files in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)
add_executable(Birthday_Simulation ${SOURCES} "${GENERATED_C}")

target_include_directories(Birthday_Simulation PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

find_package(OpenSSL REQUIRED)
 
# --- Find GLib using pkg-config ---
find_package(PkgConfig REQUIRED)

# Find GLib-2.0
pkg_check_modules(GLIB2 REQUIRED glib-2.0)

# Find ncurses libraries
pkg_check_modules(NCURSES REQUIRED ncursesw formw menuw)

# Fallback manual library finding for static linking if needed
if(USE_STATIC_LIBS)
    find_library(NCURSESW_STATIC_LIB ncursesw PATHS /usr/lib /usr/local/lib)
    find_library(FORMW_STATIC_LIB formw PATHS /usr/lib /usr/local/lib)
    find_library(MENUW_STATIC_LIB menuw PATHS /usr/lib /usr/local/lib)
    find_library(PTHREAD_STATIC_LIB pthread PATHS /usr/lib /usr/local/lib)
    find_library(TINFO_STATIC_LIB tinfo PATHS /usr/lib usr/local/lib)
endif()

# --- Link Libraries ---
if(USE_STATIC_LIBS AND NCURSESW_STATIC_LIB AND FORMW_STATIC_LIB AND MENUW_STATIC_LIB)
    # Use manually found static libraries
    target_link_libraries(Birthday_Simulation PRIVATE
        ${MENUW_STATIC_LIB}
        ${FORMW_STATIC_LIB}
        ${NCURSESW_STATIC_LIB}
        ${TINFO_STATIC_LIB}
        OpenSSL::SSL
        OpenSSL::Crypto
        ${GLIB2_STATIC_LIBRARIES}
        ${PTHREAD_STATIC_LIB}
    )
else()
    # Use pkg-config found libraries
    target_link_libraries(Birthday_Simulation PRIVATE
        ${NCURSES_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        ${GLIB2_LIBRARIES}
    )
endif()

# Add GLib include directories and compile flags
target_include_directories(Birthday_Simulation PRIVATE ${GLIB2_INCLUDE_DIRS})
target_compile_options(Birthday_Simulation PRIVATE ${GLIB2_CFLAGS_OTHER})

# Add library directories for GLib
target_link_directories(Birthday_Simulation PRIVATE ${GLIB2_LIBRARY_DIRS})

# Custom output directory
set_target_properties(Birthday_Simulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)