cmake_minimum_required(VERSION 3.3)
project(serializers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(BUILD_JSON ON)
set(BUILD_DCM ON)

set(SOURCE_FILES main.cpp)

include_directories(include)

add_executable(serializers ${SOURCE_FILES} include/serializer.hpp serializers/json.hpp serializers/json.cpp)

if (BUILD_JSON)
    find_package(JsonCpp REQUIRED)
    add_library(${PROJECT_NAME}_json SHARED serializers/json.hpp serializers/json.cpp)
    target_link_libraries(${PROJECT_NAME}_json ${JSONCPP_LIBRARIES})
    target_link_libraries(serializers  ${JSONCPP_LIBRARIES})
endif()

if (BUILD_DCM)
    find_package(DcmIpc REQUIRED)
    add_library(${PROJECT_NAME}_dcmbuf SHARED serializers/dcm_buf.cpp serializers/dcm_buf.hpp)
    target_link_libraries(serializers  ${DCMIPC_LIBRARIES})
ENDIF()