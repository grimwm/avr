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

add_custom_target(projfiles_extra SOURCES
  README.md)

if(AVR)
  include_directories(libavr)
  
  add_avr_fuse_target()

  add_executable(servo servo.c)
  add_avr_to_target(servo)

  add_executable(pwm pwm.c)
  add_avr_to_target(pwm)

  add_executable(uart_echo uart_echo.c)
  add_avr_to_target(uart_echo)

  add_executable(blinky blinky.c)
  add_avr_to_target(blinky)

  add_definitions(-DBAUD=38400)
else(AVR)
  add_executable(master master.c)
  
  add_executable(pwmmaster pwmmaster.c)
endif(AVR)
