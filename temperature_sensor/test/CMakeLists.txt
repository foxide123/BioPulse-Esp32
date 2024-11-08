# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/version.cmake)

# Add newly added components to one of these lines:
# 1. Add here if the component is compatible with IDF >= v4.3
set(EXTRA_COMPONENT_DIRS "../components")

set(TEST_COMPONENTS "ds18b20" "i2c" CACHE STRING "List of components to test")


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(temperature_sensor_test)

add_subdirectory(../external_components/fff ${CMAKE_BINARY_DIR}/external_components/fff)
