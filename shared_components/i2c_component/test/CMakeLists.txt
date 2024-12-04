set(UNIT_TESTING 1)
idf_component_register(SRCS "i2c_slave_test.c" "mocks/i2c_mock.c" "../src/i2c_master.c" "../src/i2c_slave.c"
                INCLUDE_DIRS "../include" "../../../external_components/fff"
                REQUIRES unity cmock driver esp_driver_i2c i2c freertos)
target_link_libraries(${COMPONENT_LIB} PUBLIC fff)
target_compile_definitions(${COMPONENT_LIB} PRIVATE UNIT_TESTING)

if(${IDF_TARGET} STREQUAL "linux")
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_IDF_TARGET_LINUX=1)
endif()