cmake_minimum_required(VERSION 3.2)
project(bpstructure)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

option(BUILD_JSON "build json serializer" ON)
option(BUILD_DCM "build dcm serializer" OFF)
option(BUILD_TEST "build tests" ON)
option(USE_BOOST_VARIANT "use boost variant" ON)

set(BpStructure_VERSION_MAJOR 1)
set(BpStructure_VERSION_MINOR 0)
set(BpStructure_VERSION_PATCH 0)
set(BpStructure_VERSION ${BpStructure_VERSION_MAJOR}.${BpStructure_VERSION_MINOR}.${BpStructure_VERSION_PATCH} )

set(SOURCE_FILES main.cpp serializers/structure.cpp include/structure.hpp serializers/structure_iterators.cpp include/variant.hpp)
set(TEST_FILES tests/test_struct.cpp serializers/structure.cpp include/structure.hpp serializers/structure_iterators.cpp include/variant.hpp)


find_package(BpUtil 1.0 REQUIRED)
include_directories(include )

if (USE_BOOST_VARIANT)
    add_definitions(-DUSE_BOOST_VARIANT)
endif()

#add_executable(serializers ${SOURCE_FILES})
if (BUILD_TEST)
    add_executable(${PROJECT_NAME}Test ${TEST_FILES})
endif()

add_library(${PROJECT_NAME} SHARED serializers/structure.cpp serializers/structure_iterators.cpp)
target_link_libraries(${PROJECT_NAME})

if (BUILD_TEST)
    target_link_libraries(${PROJECT_NAME}Test  gtest pthread gtest_main )
endif()

if (BUILD_JSON)
    find_package(JsonCpp REQUIRED)
    include_directories(${JSONCPP_INCLUDE_DIRS}/jsoncpp)
    set(JSON_SOURCES include/serializers/json.hpp serializers/json.cpp)
    add_library(bpserializers_json SHARED ${JSON_SOURCES})
    set_target_properties(bpserializers_json PROPERTIES VERSION ${BpStructure_VERSION} SOVERSION ${BpStructure_VERSION_MAJOR} )
    target_link_libraries(bpserializers_json ${JSONCPP_LIBRARIES} ${PROJECT_NAME} )
    install(TARGETS bpserializers_json DESTINATION lib/binelpro/)
    install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "serializers/json.hpp")
#    target_link_libraries(serializers bpserializers_json)
endif()

if (BUILD_DCM)
    find_package(DcmIpc REQUIRED)
    set(DCM_SOURCES serializers/dcm_buf.cpp include/serializers/dcm_buf.hpp)
    add_library(bpserializers_dcmbuf SHARED ${DCM_SOURCES})
    set_target_properties(bpserializers_dcmbuf PROPERTIES VERSION ${BpStructure_VERSION} SOVERSION ${BpStructure_VERSION_MAJOR} )
    target_link_libraries(bpserializers_dcmbuf ${PROJECT_NAME} )
    install(TARGETS bpserializers_dcmbuf DESTINATION lib/binelpro/)
    install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "serializers/dcm_buf.hpp")
#    target_link_libraries(serializers bpserializers_dcmbuf)
ENDIF()

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${BpStructure_VERSION} SOVERSION ${BpStructure_VERSION_MAJOR} )

set(CMAKECONFIG_INSTALL_DIR "lib/cmake/bpstructure")

install(TARGETS ${PROJECT_NAME} DESTINATION lib/binelpro/)
install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "*.hpp")
install(FILES "cmake/BpStructureConfig.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/BpStructureConfigVersion.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/ExtLibs.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})