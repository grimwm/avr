cmake_minimum_required(VERSION 2.6)

Project(AVR)

# add_subdirectory(blink_led)
add_subdirectory(lib)

include_directories(lib)
add_executable(pwm pwm.c)
if(AVR)
  add_avr_to_target(pwm)
endif(AVR)
