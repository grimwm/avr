cmake_minimum_required(VERSION 2.6)

Project(AVR)

add_subdirectory(blink_led)
add_subdirectory(libpwm)

add_executable(pwm pwm.c)