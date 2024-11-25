cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/version.cmake)

set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS "../i2c")
set(EXTRA_COMPONENT_DIRS "../cJSON")
set(TEST_COMPONENTS "cJSON" "i2c" CACHE STRING "List of components to test")


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(temperature_sensor_test)

add_subdirectory(../fff ${CMAKE_BINARY_DIR}/fff)


add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/build/coverage.info"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
    COMMAND lcov --capture --directory . --output-file coverage.info
    COMMENT "Create coverage report"
    )

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/build/coverage_report/"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/build/coverage.info"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
    COMMAND genhtml coverage.info --output-directory coverage_report/
    COMMENT "Turn coverage report into html-based visualization"
    )

add_custom_target(coverage
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
    DEPENDS "coverage_report/"
    )