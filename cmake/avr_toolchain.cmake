set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_CROSSCOMPILING ON)

set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_OBJCOPY avr-objcopy CACHE STRING "")
set(CMAKE_OBJDUMP avr-objdump CACHE STRING "")
set(CMAKE_NM avr-nm CACHE STRING "")

set(AVRDUDE sudo avrdude)

set(AVR ON CACHE BOOL "")

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(CDEBUG "-gstabs")
endif()

set(CWARN "-Wall -Wstrict-prototypes -Werror")
set(CXXWARN "-Wall -Werror")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
set(COPT "-Os -mcall-prologues")

set(
  CMAKE_C_FLAGS
  "-mmcu=${MCU} -DF_CPU=${F_CPU} -std=c99 ${CDEBUG} ${COPT} ${CWARN} ${CFLAGS}"
  CACHE STRING "")# FORCE)

set(
  CMAKE_CXX_FLAGS
  "-mmcu=${MCU} -DF_CPU=${F_CPU} -std=c++11 ${CDEBUG} ${COPT} ${CXXWARN} ${CXXFLAGS}"
  CACHE STRING "")# FORCE)

set(FUSE "-U lfuse:w:0xe2:m -U hfuse:w:0xde:m -U efuse:w:0x00:m")

macro(add_avr_to_target target_name)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${target_name} ${target_name}.hex)

  add_custom_target(install
    COMMAND ${AVRDUDE} -p ${MCU} -c linuxgpio -U flash:w:${target_name}.hex
    DEPENDS ${target_name})

  add_custom_target(fuse
    COMMAND ${AVRDUDE} -p ${MCU} -c linuxgpio ${FUSE})

  set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    ${target_name}.hex)
endmacro(add_avr_to_target)
