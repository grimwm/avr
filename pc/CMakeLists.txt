cmake_minimum_required(VERSION 2.6)

Project(AVR-pc)

file(GLOB_RECURSE PROJHEADERS *.h)
add_custom_target(projheaders SOURCES ${PROJHEADERS})

add_custom_target(projfiles SOURCES ../README.md)

include_directories(lib)
add_subdirectory(lib)

add_executable(hexuploader hexuploader.c)
target_link_libraries(hexuploader io)
  
add_executable(jsmaster jsmaster.c)
target_link_libraries(jsmaster io joystick)

add_executable(jstest jstest.c)
