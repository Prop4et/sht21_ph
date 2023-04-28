add_library(ph-lib INTERFACE)
target_include_directories(ph-lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ph-lib INTERFACE pico_stdlib hardware_adc)
target_sources(ph-lib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ph_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/ph_driver.h
)

