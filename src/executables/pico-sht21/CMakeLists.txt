cmake_minimum_required(VERSION 3.12)

# rest of your project
add_executable(pico-sht21
  main.c
)

# pull in common dependencies

target_link_libraries(pico-sht21
    sht21-lib
    pico_stdlib
    hardware_i2c
    pico_stdio_usb
    hardware_rtc
    hardware_sleep
    pico_runtime  
)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico-sht21 0)
pico_enable_stdio_uart(pico-sht21 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pico-sht21)
