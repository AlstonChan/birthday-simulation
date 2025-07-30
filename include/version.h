#ifndef VERSION_H
#define VERSION_H

#define PROGRAM_VERSION_MAJOR PROJECT_VERSION_MAJOR
#define PROGRAM_VERSION_MINOR PROJECT_VERSION_MINOR
#define PROGRAM_VERSION_PATCH PROJECT_VERSION_PATCH

// Use preprocessor stringification to create the version string
#define _VERSION_STRINGIFY(x) #x
#define _VERSION_TO_STRING(major, minor, patch)                                                    \
    _VERSION_STRINGIFY(major) "." _VERSION_STRINGIFY(minor) "." _VERSION_STRINGIFY(patch)

#define PROGRAM_VERSION_STRING                                                                     \
    _VERSION_TO_STRING(PROGRAM_VERSION_MAJOR, PROGRAM_VERSION_MINOR, PROGRAM_VERSION_PATCH)

#endif