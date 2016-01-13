cmake_minimum_required(VERSION 3.2)
project(bpserializers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(BUILD_JSON ON)
set(BUILD_DCM ON)

set(SOURCE_FILES main.cpp serializers/serializer.cpp)

find_package(BpUtil REQUIRED)

include_directories(include)

add_executable(serializers ${SOURCE_FILES} include/serializer.hpp serializers/json.hpp serializers/json.cpp serializers/dcm_buf.cpp)

add_library(${PROJECT_NAME} SHARED serializers/serializer.cpp)

if (BUILD_JSON)
    find_package(JsonCpp REQUIRED)
    add_library(${PROJECT_NAME}_json SHARED serializers/json.hpp serializers/json.cpp)
    target_link_libraries(${PROJECT_NAME}_json ${JSONCPP_LIBRARIES})
    target_link_libraries(serializers  ${JSONCPP_LIBRARIES})
    install(TARGETS ${PROJECT_NAME}_json DESTINATION lib/binelpro/)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_json)
endif()

if (BUILD_DCM)
    find_package(DcmIpc REQUIRED)
    add_library(${PROJECT_NAME}_dcmbuf SHARED serializers/dcm_buf.cpp serializers/dcm_buf.hpp)
    target_link_libraries(serializers  ${DCMIPC_LIBRARIES} ${BPUTIL_LIBRARIES})
    target_link_libraries(${PROJECT_NAME}_dcmbuf ${BPUTIL_LIBRARIES})
    install(TARGETS ${PROJECT_NAME}_dcmbuf DESTINATION lib/binelpro/)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_dcmbuf)
ENDIF()

set(CMAKECONFIG_INSTALL_DIR "lib/cmake/bpserializers")

install(TARGETS ${PROJECT_NAME} DESTINATION lib/binelpro/)
install(DIRECTORY include/ DESTINATION include/binelpro/ FILES_MATCHING PATTERN "*.hpp")
install(FILES "cmake/BpSerializersConfig.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/BpSerializersVersion.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES "cmake/ExtLibs.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})