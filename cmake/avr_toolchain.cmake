set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_CROSSCOMPILING ON)

set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_OBJCOPY avr-objcopy CACHE STRING "")
set(CMAKE_OBJDUMP avr-objdump CACHE STRING "")
set(CMAKE_NM avr-nm CACHE STRING "")

set(AVR ON CACHE BOOL "")

if(AVR_TARGET STREQUAL "avr")
  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(CDEBUG "-gstabs")
  endif()
  
  set(CWARN "-Wall -Wstrict-prototypes -Werror")
  set(CXXWARN "-Wall -Wstrict-prototypes -Werror")
  set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
  set(COPT "-Os")

  set(
    CMAKE_C_FLAGS
    "${CDEBUG} ${COPT} ${CWARN} ${CFLAGS}"
    CACHE STRING "")# FORCE)
  
  set(
    CMAKE_CXX_FLAGS
    "${CDEBUG} ${COPT} ${CXXWARN} ${CXXFLAGS}"
    CACHE STRING "")# FORCE)
endif()
