cmake_minimum_required(VERSION 2.6)

Project(AVR)

# Currently, this is hard-coded to a system path for headers,
# but a clever update to the ./configure script can use
#   echo | gcc -v -x c -E - 2>&1 | grep '^ /.*/include'
# to turn this into a convenient macro that works across machines
# and (cross-)compilers.
file(GLOB_RECURSE SYSHEADERS /usr/lib/avr/include/*.h)
add_custom_target(sysheaders SOURCES ${SYSHEADERS})

file(GLOB_RECURSE PROJHEADERS *.h)
add_custom_target(projheaders SOURCES ${PROJHEADERS})

add_custom_target(projfiles SOURCES ../README.md)

include_directories(lib)
add_subdirectory(lib)

add_avr_fuse_target()

add_avr_executable(bootloader bootloader.c)
set_property(TARGET bootloader APPEND PROPERTY
  COMPILE_DEFINITIONS BOOT_TIMEOUT_MS=10000)
set_target_properties(bootloader PROPERTIES LINK_FLAGS
  -Wl,--section-start=.text=${BOOTSTARTB})
target_link_libraries(bootloader io)
add_avr_install_target(bootloader)

add_avr_executable(servo servo.c)
target_link_libraries(servo io)
add_avr_install_target(servo)

add_avr_executable(pwm pwm.c)
target_link_libraries(pwm io)
add_avr_install_target(pwm)

add_avr_executable(uart_echo uart_echo.c)
target_link_libraries(uart_echo io)
add_avr_install_target(uart_echo)

add_avr_executable(blinky blinky.c)
add_avr_install_target(blinky)
