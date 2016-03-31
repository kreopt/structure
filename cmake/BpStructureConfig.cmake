# - Try to find DCM IPC
# Once done this will define
#  BPSTRUCTURE_FOUND - System has DCM IPC
#  BPSTRUCTURE_INCLUDE_DIRS - The DCM IPC include directories
#  BPSTRUCTURE_LIBRARIES - The libraries needed to use DCM IPC
#  BPSTRUCTURE_DEFINITIONS - Compiler switches required for using DCM IPC

set(CMAKECONFIG_INSTALL_DIR "lib/cmake/bpstructure")

find_path(BPSTRUCTURE_INCLUDE_DIRS
        NAMES binelpro/structure.hpp
        DOC "binelpro structure dir"
        PATHS /usr/local/include /usr/include
        )

find_library(
        BPSTRUCTURE_LIBRARIES
        NAMES bpstructure
        DOC "bpstructure library"
        PATH_SUFFIXES binelpro/
)
find_library(
        BPSTRUCTURE_LIBRARIES_JSON
        NAMES bpserializers_json
        DOC "JSON serializer library"
        PATH_SUFFIXES binelpro/
)
find_library(
        BPSTRUCTURE_LIBRARIES_DCM
        NAMES bpserializers_dcmbuf
        DOC "DCM serializer library"
        PATH_SUFFIXES binelpro/
)
include(${BPSTRUCTURE_INCLUDE_DIRS}/../${CMAKECONFIG_INSTALL_DIR}/ExtLibs.cmake)

mark_as_advanced(BPSTRUCTURE_INCLUDE_DIR OS_SPECIFIC)
