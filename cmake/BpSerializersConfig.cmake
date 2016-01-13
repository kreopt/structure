# - Try to find DCM IPC
# Once done this will define
#  BPSERIALIZERS_FOUND - System has DCM IPC
#  BPSERIALIZERS_INCLUDE_DIRS - The DCM IPC include directories
#  BPSERIALIZERS_LIBRARIES - The libraries needed to use DCM IPC
#  BPSERIALIZERS_DEFINITIONS - Compiler switches required for using DCM IPC

set(BPSERIALIZERS_INSTALL_PREFIX "/usr/local")
set(CMAKECONFIG_INSTALL_DIR "lib/cmake/bpserializers")

find_path(BPSERIALIZERS_INCLUDE_DIR ${BPUTIL_INSTALL_PREFIX}/binelpro/serializer.hpp PATH_SUFFIXES binelpro )

#include(${BPSERIALIZERS_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR}/ExtLibs.cmake)

set(BPSERIALIZERS_INCLUDE_DIRS, ${BPSERIALIZERS_INCLUDE_DIR})
set(BPSERIALIZERS_LIBRARIES  ${BPSERIALIZERS_EXT_LIBRARIES} ${BPSERIALIZERS_INSTALL_PREFIX}/lib/binelpro/libbpserializers.so)
set(BPSERIALIZERS_LIBRARIES_JSON  ${BPSERIALIZERS_INSTALL_PREFIX}/lib/binelpro/libbpserializers_json.so)
set(BPSERIALIZERS_LIBRARIES_DCM  ${BPSERIALIZERS_INSTALL_PREFIX}/lib/binelpro/libbpserializers_dcmbuf.so)

mark_as_advanced(BPSERIALIZERS_INCLUDE_DIR OS_SPECIFIC)