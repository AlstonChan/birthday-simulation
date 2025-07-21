#ifndef VERSION_H
#define VERSION_H

#define PROGRAM_VERSION_MAJOR 1
#define PROGRAM_VERSION_MINOR 0
#define PROGRAM_VERSION_PATCH 0

// Use preprocessor stringification to create the version string
#define _VERSION_STRINGIFY(x) #x
#define _VERSION_TO_STRING(major, minor, patch)                                                    \
    _VERSION_STRINGIFY(major) "." _VERSION_STRINGIFY(minor) "." _VERSION_STRINGIFY(patch)

#define PROGRAM_VERSION_STRING                                                                     \
    _VERSION_TO_STRING(PROGRAM_VERSION_MAJOR, PROGRAM_VERSION_MINOR, PROGRAM_VERSION_PATCH)

#endif