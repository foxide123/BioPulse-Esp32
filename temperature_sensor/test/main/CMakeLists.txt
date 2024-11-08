idf_component_register(SRCS "unit_test.c"
                    INCLUDE_DIRS "." "../../external_components/fff")
target_link_libraries(${COMPONENT_LIB} PUBLIC fff)