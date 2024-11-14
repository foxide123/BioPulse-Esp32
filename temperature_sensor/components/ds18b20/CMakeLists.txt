idf_component_register(SRCS "src/ds18b20.c"
                INCLUDE_DIRS "include"
                REQUIRES unity
                PRIV_REQUIRES esp_driver_gpio)
                

if(${IDF_TARGET} STREQUAL "linux")
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_IDF_TARGET_LINUX=1)
elseif(${IDF_TARGET} STREQUAL "esp32")
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_IDF_TARGET_ESP32=1)
else()
    idf_component_get_property(driver_dir driver COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${driver_dir}/include")
    idf_component_get_property(esp_rom_dir esp_rom COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${esp_rom_dir}/include")
    idf_component_get_property(driver_lib driver COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PRIVATE "${driver_lib}")
endif()