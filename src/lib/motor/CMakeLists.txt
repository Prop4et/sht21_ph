add_library(motor-lib INTERFACE)
target_include_directories(motor-lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(motor-lib INTERFACE pico_stdlib hardware_pwm)
target_sources(motor-lib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/motor_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/motor_driver.h
)

