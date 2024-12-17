# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

get_filename_component(SHARED_COMPONENTS_DIR "${CMAKE_SOURCE_DIR}/../shared_components" ABSOLUTE)
message(STATUS "Resolved SHARED_COMPONENTS_DIR: ${SHARED_COMPONENTS_DIR}")

set(EXTRA_COMPONENT_DIRS "${SHARED_COMPONENTS_DIR}")
message(STATUS "EXTRA_COMPONENT_DIRS: ${EXTRA_COMPONENT_DIRS}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(peristaltic_pump)

message(STATUS "Debug message: CMake is running")
message(STATUS "CMAKE_SOURCE_DIR is: ${CMAKE_SOURCE_DIR}")

set(COMPONENTS main)

list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/driver/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/esp_partition/")

if(UNIT_TESTING)
    list(APPEND EXTRA_COMPONENT_DIRS "${SHARED_COMPONENTS_DIR}/i2c_component/test")
endif()
