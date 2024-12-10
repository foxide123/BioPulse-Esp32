idf_component_register(SRCS "main.c"
            INCLUDE_DIRS "."
            REQUIRES esp_driver_i2c ds18b20 temperature i2c_component mqtt_component nvs_flash
            PRIV_REQUIRES esp_driver_gpio)
            
if(${IDF_TARGET} STREQUAL "linux")
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_IDF_TARGET_LINUX=1)
else()
    idf_component_get_property(esp_driver_gpio_dir esp_driver_gpio COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${esp_driver_gpio_dir}/include")

    idf_component_get_property(esp_driver_gpio_dir esp_driver_gpio COMPONENT_DIR)
    target_link_libraries(${COMPONENT_LIB} PRIVATE "${esp_driver_gpio_dir}")
endif()