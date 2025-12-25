cmake_minimum_required(VERSION 3.13...3.27)


set(PICO_SDK_FETCH_FROM_GIT on)


# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(divider C CXX ASM)


# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(divider divider.c)
pico_generate_pio_header(divider quadrature.pio)
target_link_libraries(divider pico_stdlib hardware_pio)
pico_add_extra_outputs(divider)

pico_enable_stdio_usb(divider 1)
pico_enable_stdio_uart(divider 0)


