include_directories(../mylib)

add_executable(blink_led blink.c)
target_link_libraries(blink_led mylib)


if(AVR)
add_custom_command(TARGET blink_led
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex -R eeprom blink_led blink_led.hex
  )
endif(AVR)
