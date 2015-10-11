add_library(io STATIC io.c serial.c)

add_library(joystick joystick.c)
target_link_libraries(joystick json)
