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
    message(STATUS "Configuring for WIN32 (MSYS2/MinGW)")

    if(USE_STATIC_LIBS)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -Bstatic -Wl,-Bdynamic")
    endif()

    set(CMAKE_PREFIX_PATH
        "C:/msys64/ucrt64"
        "/ucrt64/include"
        "/ucrt64"
        "/usr/local"
    )
else()
    message(STATUS "Configuring for POSIX (Linux/Debian)")

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

if(WIN32)
    target_include_directories(
        Birthday_Simulation
        PRIVATE ${CMAKE_SOURCE_DIR}/include
        "C:/msys64/ucrt64/include/ncursesw"
        ${CMAKE_CURRENT_BINARY_DIR}
    )
else()
    target_include_directories(
        Birthday_Simulation
        PRIVATE ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)

if(USE_STATIC_LIBS)
    target_link_options(Birthday_Simulation PRIVATE
        -Wl,-Bstatic
        -lglib-2.0 -lintl -lpcre2-8
        -Wl,-Bdynamic
        -lws2_32 -lole32 -lwinmm -lshlwapi -luuid -lm
    )

    # Add static compilation flags
    target_compile_definitions(Birthday_Simulation PRIVATE
        GLIB_STATIC_COMPILATION
        G_DISABLE_DEPRECATED
    )

    if(WIN32)
        set(GLIB2_STATIC_LIBS
            "C:/msys64/ucrt64/lib/libglib-2.0.a"
            "C:/msys64/ucrt64/lib/libpcre2-8.a"
            "C:/msys64/ucrt64/lib/libintl.a"
            "C:/msys64/ucrt64/lib/libiconv.a"
        )

        set(GLIB2_SYSTEM_LIBS
            ws2_32
            ole32
            winmm
            shlwapi
            uuid
            m
        )
    else()
        set(GLIB2_STATIC_LIBS
            "/usr/lib/x86_64-linux-gnu/libglib-2.0.a" # or /usr/local/lib/
            "/usr/lib/x86_64-linux-gnu/libpcre2-8.a" # or /usr/local/lib/
        )

        set(GLIB2_SYSTEM_LIBS
            m # math library
        )

        # Note: On Linux, you typically don't need libintl and libiconv
        # as they're part of glibc, but check if they exist
        if(EXISTS "/usr/lib/x86_64-linux-gnu/libintl.a")
            list(APPEND GLIB2_STATIC_LIBS "/usr/lib/x86_64-linux-gnu/libintl.a")
        endif()
    endif()

    # Check if the static libraries exist
    foreach(lib ${GLIB2_STATIC_LIBS})
        if(NOT EXISTS ${lib})
            message(FATAL_ERROR "Static library not found: ${lib}")
        endif()
    endforeach()

    message(STATUS "Using explicit static GLib libraries: ${GLIB2_STATIC_LIBS}")
else()
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)
endif()

# Always get include directories
pkg_check_modules(GLIB2_INCLUDES REQUIRED glib-2.0)
target_include_directories(Birthday_Simulation PRIVATE ${GLIB2_INCLUDES_INCLUDE_DIRS})

pkg_check_modules(NCURSES REQUIRED ncursesw formw menuw)

# Fallback manual library finding for static linking if needed
if(USE_STATIC_LIBS)
    find_library(NCURSESW_STATIC_LIB ncursesw PATHS /usr/lib /usr/local/lib)
    find_library(FORMW_STATIC_LIB formw PATHS /usr/lib /usr/local/lib)
    find_library(MENUW_STATIC_LIB menuw PATHS /usr/lib /usr/local/lib)
    find_library(PTHREAD_STATIC_LIB pthread PATHS /usr/lib /usr/local/lib)

    if(LINUX)
        find_library(TINFO_STATIC_LIB tinfo PATHS /usr/lib usr/local/lib)
    endif()
endif()

# --- Link Libraries ---
if(USE_STATIC_LIBS AND NCURSESW_STATIC_LIB AND FORMW_STATIC_LIB AND MENUW_STATIC_LIB)
    # Use manually found static libraries
    if(LINUX)
        target_link_libraries(Birthday_Simulation PRIVATE
            ${MENUW_STATIC_LIB}
            ${FORMW_STATIC_LIB}
            ${NCURSESW_STATIC_LIB}
            ${TINFO_STATIC_LIB}
            OpenSSL::SSL
            OpenSSL::Crypto
            ${GLIB2_STATIC_LIBS} # Full paths to .a files
            ${GLIB2_SYSTEM_LIBS} # Sy
            ${PTHREAD_STATIC_LIB}
        )
    else()
        target_link_libraries(Birthday_Simulation PRIVATE
            ${MENUW_STATIC_LIB}
            ${FORMW_STATIC_LIB}
            ${NCURSESW_STATIC_LIB}
            OpenSSL::SSL
            OpenSSL::Crypto
            ${GLIB2_STATIC_LIBS} # Full paths to .a files
            ${GLIB2_SYSTEM_LIBS} # Sy
            ${PTHREAD_STATIC_LIB}
        )
    endif()
else()
    # Use pkg-config found libraries
    target_link_libraries(Birthday_Simulation PRIVATE
        ${NCURSES_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        ${GLIB2_LIBRARIES}
    )
endif()

# Custom output directory
set_target_properties(Birthday_Simulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)