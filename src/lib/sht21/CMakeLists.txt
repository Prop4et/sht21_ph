add_library(sht21-lib INTERFACE)
target_include_directories(sht21-lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(sht21-lib INTERFACE pico_stdlib hardware_i2c)
target_sources(sht21-lib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/sht21.c
    ${CMAKE_CURRENT_LIST_DIR}/sht21.h
)

