idf_component_register(SRCS "temperature_tests_main.c"
                    #INCLUDE_DIRS "." "../../external_components/fff"
                    INCLUDE_DIRS "."
                    REQUIRES cmock
                    PRIV_REQUIRES esp_driver_gpio)
#target_link_libraries(${COMPONENT_LIB} PUBLIC fff)
target_compile_definitions(${COMPONENT_LIB} PRIVATE UNIT_TESTING)
