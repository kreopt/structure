cmake_minimum_required(VERSION 3.2)
project(bpstructure)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(BUILD_JSON ON)
set(BUILD_DCM ON)

set(SOURCE_FILES main.cpp serializers/structure.cpp include/structure.hpp serializers/structure_iterators.cpp)

if (BUILD_JSON)
    set(JSON_SOURCES include/serializers/json.hpp serializers/json.cpp)
endif()

if (BUILD_DCM)
    set(DCM_SOURCES serializers/dcm_buf.cpp include/serializers/dcm_buf.hpp)
endif()

find_package(BpUtil REQUIRED)

include_directories(include)

add_executable(serializers ${SOURCE_FILES})

add_library(${PROJECT_NAME} SHARED serializers/structure.cpp serializers/structure_iterators.cpp)
target_link_libraries(${PROJECT_NAME} ${BPUTIL_LIBRARIES})


if (BUILD_JSON)
    find_package(JsonCpp REQUIRED)
    add_library(bpserializers_json SHARED ${JSON_SOURCES})
    target_link_libraries(bpserializers_json ${JSONCPP_LIBRARIES} ${PROJECT_NAME} )
    install(TARGETS bpserializers_json DESTINATION lib/binelpro/)
    install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "serializers/json.hpp")
    target_link_libraries(serializers bpserializers_json)
endif()

if (BUILD_DCM)
    find_package(DcmIpc REQUIRED)
    add_library(bpserializers_dcmbuf SHARED ${DCM_SOURCES})
    target_link_libraries(bpserializers_dcmbuf ${BPUTIL_LIBRARIES} ${PROJECT_NAME} )
    install(TARGETS bpserializers_dcmbuf DESTINATION lib/binelpro/)
    install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "serializers/dcm_buf.hpp")
    target_link_libraries(serializers bpserializers_dcmbuf)
ENDIF()

set(CMAKECONFIG_INSTALL_DIR "lib/cmake/bpstructure")

install(TARGETS ${PROJECT_NAME} DESTINATION lib/binelpro/)
install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "*.hpp")
install(FILES "cmake/BpStructureConfig.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/BpStructureVersion.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/ExtLibs.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})