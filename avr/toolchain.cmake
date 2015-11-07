set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_CROSSCOMPILING ON)

set(CMAKE_ASM_COMPILER avr-as)
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

# BAUD_TOL is set to a large value because we are going to trust the developer
# to make a good decision, especially if they're using crystal oscillators
# or external clocks.
set(
  CMAKE_C_FLAGS
  "-mmcu=${MCU} -DF_CPU=${F_CPU} -DBAUD=${BAUD} -std=c11 ${CDEBUG} ${COPT} ${CWARN} ${CFLAGS}"
  CACHE STRING "")# FORCE)

set(
  CMAKE_CXX_FLAGS
  "-mmcu=${MCU} -DF_CPU=${F_CPU} -std=c++11 ${CDEBUG} ${COPT} ${CXXWARN} ${CXXFLAGS}"
  CACHE STRING "")# FORCE)

# Internal RC oscillator, default settings.
# Clock NOT divided by 8
# 2.7v bod
# set(FUSE -U lfuse:w:0xe2:m -U hfuse:w:0xde:m -U efuse:w:0x00:m)

# Full-swing crystal oscillator, slowly rising power.
# Clock NOT divided by 8
# 2.7v bod
set(FUSE -U lfuse:w:0xf7:m -U hfuse:w:0xde:m -U efuse:w:0x00:m)

macro(_add_avr_post_build target_name)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${target_name} ${target_name}.hex)

  set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    ${target_name}.hex)
endmacro(_add_avr_post_build)

macro(add_avr_executable target_name srcs)
  add_executable(${target_name} ${srcs})
  _add_avr_post_build(${target_name})
endmacro(add_avr_executable)

macro(add_avr_install_target target_name)
  add_custom_target(install_${target_name}
    COMMAND ${AVRDUDE} -p ${MCU} -c linuxgpio -U flash:w:${target_name}.hex -U lock:w:0x3f:m
    DEPENDS ${target_name})
endmacro(add_avr_install_target)

macro(add_avr_fuse_target)
  add_custom_target(fuse
    COMMAND ${AVRDUDE} -p ${MCU} -c linuxgpio ${FUSE})
endmacro(add_avr_fuse_target)
