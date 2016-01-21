cmake_minimum_required(VERSION 3.2)
project(bpserializers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(BUILD_JSON ON)
set(BUILD_DCM ON)

set(SOURCE_FILES main.cpp serializers/serializer.cpp include/serializer.hpp serializers/serializer_iterators.cpp)

if (BUILD_JSON)
    set(JSON_SOURCES include/serializers/json.hpp serializers/json.cpp)
endif()

if (BUILD_DCM)
    set(DCM_SOURCES serializers/dcm_buf.cpp include/serializers/dcm_buf.hpp)
endif()

find_package(BpUtil REQUIRED)

include_directories(include)

add_executable(serializers ${SOURCE_FILES} ${JSON_SOURCES} ${DCM_SOURCES})

add_library(${PROJECT_NAME} SHARED serializers/serializer.cpp)

if (BUILD_JSON)
    find_package(JsonCpp REQUIRED)
    add_library(${PROJECT_NAME}_json SHARED ${JSON_SOURCES})
    target_link_libraries(${PROJECT_NAME}_json ${JSONCPP_LIBRARIES})
    target_link_libraries(serializers  ${JSONCPP_LIBRARIES})
    install(TARGETS ${PROJECT_NAME}_json DESTINATION lib/binelpro/)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_json)
    install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "serializers/json.hpp")
endif()

if (BUILD_DCM)
    find_package(DcmIpc REQUIRED)
    add_library(${PROJECT_NAME}_dcmbuf SHARED ${DCM_SOURCES})
    target_link_libraries(serializers  ${DCMIPC_LIBRARIES} ${BPUTIL_LIBRARIES})
    target_link_libraries(${PROJECT_NAME}_dcmbuf ${BPUTIL_LIBRARIES})
    install(TARGETS ${PROJECT_NAME}_dcmbuf DESTINATION lib/binelpro/)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_dcmbuf)
    install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "serializers/dcm_buf.hpp")
ENDIF()

set(CMAKECONFIG_INSTALL_DIR "lib/cmake/bpserializers")

install(TARGETS ${PROJECT_NAME} DESTINATION lib/binelpro/)
install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "*.hpp")
install(FILES "cmake/BpSerializersConfig.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/BpSerializersVersion.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/ExtLibs.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})