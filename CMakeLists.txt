cmake_minimum_required(VERSION 2.6)

Project(AVR)

# add_subdirectory(blink_led)
add_subdirectory(lib)

include_directories(lib)
if(AVR)
  add_executable(pwm pwm.c)
  add_avr_to_target(pwm)
else(AVR)
  add_executable(master master.c)
endif(AVR)
