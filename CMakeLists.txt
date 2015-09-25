cmake_minimum_required(VERSION 2.6)

Project(AVR)

# add_subdirectory(blink_led)
add_subdirectory(lib)

# Currently, this is hard-coded to a system path for headers,
# but a clever update to the ./configure script can use
#   echo | gcc -v -x c -E - 2>&1 | grep '^ /.*/include'
# to turn this into a convenient macro that works across machines
# and (cross-)compilers.
file(GLOB_RECURSE SYSHEADERS /usr/lib/avr/include/*.h *.h)
add_custom_target(sysheaders SOURCES ${SYSHEADERS})

add_custom_target(projfiles_extra SOURCES
  README.md)

include_directories(lib)
if(AVR)
  add_avr_fuse_target()

  add_executable(pwm pwm.c)
  add_avr_to_target(pwm)

  add_executable(uart_echo uart_echo.c)
  add_avr_to_target(uart_echo)

  add_definitions(-DBAUD=9600)
else(AVR)
  add_executable(master master.c)
endif(AVR)
