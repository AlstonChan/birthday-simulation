cmake_minimum_required(VERSION 3.10)

project(Birthday_Simulation LANGUAGES C VERSION 1.0.0)
message("CMAKE_PROJECT_VERSION = ${CMAKE_PROJECT_VERSION}")

add_compile_definitions(
    PROJECT_VERSION_MAJOR=${Birthday_Simulation_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${Birthday_Simulation_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${Birthday_Simulation_VERSION_PATCH}
    NCURSES_STATIC # Common macro for static ncurses builds
)

configure_file(
    ${CMAKE_SOURCE_DIR}/src/explanation.md
    ${CMAKE_BINARY_DIR}/explanation.md
    COPYONLY
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Explicitly set to prefer static libraries.
# This affects find_package and general linking behavior.
set(BUILD_SHARED_LIBS OFF)

# Recursively gather all source files in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)

add_executable(Birthday_Simulation ${SOURCES})

target_include_directories(Birthday_Simulation PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- Define Library Paths for Static Linking ---
# IMPORTANT: Adjust 'C:/msys64' if your MSYS2 installation is elsewhere.
set(MSYS2_UCRT64_LIB_DIR "C:/msys64/ucrt64/lib")
set(MSYS2_UCRT64_BIN_DIR "C:/msys64/ucrt64/bin") # We will try to exclude this from linker search

set(PTHREAD_STATIC_LIB "${MSYS2_UCRT64_LIB_DIR}/libwinpthread.a")
set(NCURSESW_STATIC_LIB "${MSYS2_UCRT64_LIB_DIR}/libncursesw.a")
set(FORMW_STATIC_LIB "${MSYS2_UCRT64_LIB_DIR}/libformw.a")
set(MENUW_STATIC_LIB "${MSYS2_UCRT64_LIB_DIR}/libmenuw.a")

# --- OpenSSL Libraries (Using find_package with static preference) ---
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)

# --- Link Libraries ---
# For static linking, the order is CRUCIAL.
# Libraries that depend on others should come *before* their dependencies in the list.
target_link_libraries(Birthday_Simulation PRIVATE
    "${MENUW_STATIC_LIB}"
    "${FORMW_STATIC_LIB}"
    "${NCURSESW_STATIC_LIB}"
    OpenSSL::SSL
    OpenSSL::Crypto
    "${PTHREAD_STATIC_LIB}"
)

# --- Force Linker to Prefer Static and Exclude DLL Paths ---
# This is an attempt to explicitly tell the linker to prefer static archives
# and to avoid searching in the /bin directory for shared libraries.
# NOTE: This is highly compiler/linker dependent and might not work for all setups,
# but it's worth a try if the above isn't sufficient.
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -L${MSYS2_UCRT64_LIB_DIR} -Bstatic -Wl,-Bdynamic")

# Custom output directory
set_target_properties(Birthday_Simulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)