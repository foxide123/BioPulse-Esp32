idf_component_register(SRCS "src/i2c_master.c" "src/i2c_slave.c"
                        INCLUDE_DIRS "include"
                        REQUIRES unity cmock esp_driver_i2c esp_idf_mocks)
                        
#Config depending on the target - linux or esp32                
if(${IDF_TARGET} STREQUAL "linux")
    set(USE_MOCKS 1)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_IDF_TARGET_LINUX=1)
else()
    set(USE_MOCKS 0)
    idf_component_get_property(esp_driver_i2c_dir esp_driver_i2c COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${esp_driver_i2c_dir}/include")
    idf_component_get_property(esp_driver_i2c_lib driver COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PRIVATE "${esp_driver_i2c_lib}")
    idf_component_get_property(esp_rom_dir esp_rom COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${esp_rom_dir}/include")
endif()