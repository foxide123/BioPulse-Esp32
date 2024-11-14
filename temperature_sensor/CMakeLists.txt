cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(temperature_sensor)

set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS "../components")


list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/driver/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/esp_partition/")

add_subdirectory(external_components/fff ${CMAKE_BINARY_DIR}/external_components/fff)

if(UNIT_TESTING)
    list(APPEND EXTRA_COMPONENT_DIRS "components/i2c/test")
endif()
