cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(temperature_sensor)

set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS "")

list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/../shared_components/cJSON")
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/../shared_components/esp_idf_mocks")
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/../shared_components/fff")
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/../shared_components/i2c")


list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/driver/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/esp_partition/")

if(UNIT_TESTING)
    list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/../shared_components/i2c/test")
endif()
