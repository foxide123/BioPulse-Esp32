idf_component_register(SRCS "unit_test.c"
                    INCLUDE_DIRS "." "../../fff"
                    REQUIRES cmock
                    PRIV_REQUIRES esp_driver_gpio)
target_link_libraries(${COMPONENT_LIB} PUBLIC fff)
target_compile_definitions(${COMPONENT_LIB} PRIVATE UNIT_TESTING)
