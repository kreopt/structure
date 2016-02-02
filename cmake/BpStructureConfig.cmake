# - Try to find DCM IPC
# Once done this will define
#  BPSTRUCTURE_FOUND - System has DCM IPC
#  BPSTRUCTURE_INCLUDE_DIRS - The DCM IPC include directories
#  BPSTRUCTURE_LIBRARIES - The libraries needed to use DCM IPC
#  BPSTRUCTURE_DEFINITIONS - Compiler switches required for using DCM IPC

set(BPSTRUCTURE_INSTALL_PREFIX "/usr/local")
set(CMAKECONFIG_INSTALL_DIR "lib/cmake/bpstructure")

find_path(BPSTRUCTURE_INCLUDE_DIR ${BPUTIL_INSTALL_PREFIX}/binelpro/structure.hpp PATH_SUFFIXES binelpro )

#include(${BPSERIALIZERS_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR}/ExtLibs.cmake)

set(BPSTRUCTURE_INCLUDE_DIRS  ${BPSTRUCTURE_INCLUDE_DIR})
set(BPSTRUCTURE_LIBRARIES  ${BPSTRUCTURE_EXT_LIBRARIES} ${BPSTRUCTURE_INSTALL_PREFIX}/lib/binelpro/libbpstructure.so)
set(BPSTRUCTURE_LIBRARIES_JSON  ${BPSTRUCTURE_INSTALL_PREFIX}/lib/binelpro/libbpserializers_json.so)
set(BPSTRUCTURE_LIBRARIES_DCM  ${BPSTRUCTURE_INSTALL_PREFIX}/lib/binelpro/libbpserializers_dcmbuf.so)

mark_as_advanced(BPSTRUCTURE_INCLUDE_DIR OS_SPECIFIC)
