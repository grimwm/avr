if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(COPT "-O0 -gstabs")
else()
  set(COPT "-O2")
endif()

set(CWARN "-Wall -Wstrict-prototypes -Werror")
set(CXXWARN "-Wall -Werror")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")

set(
  CMAKE_C_FLAGS
  "-std=c99 ${COPT} ${CWARN} ${CFLAGS}"
  CACHE STRING "")# FORCE)

set(
  CMAKE_CXX_FLAGS
  "-std=c++11 ${COPT} ${CXXWARN} ${CXXFLAGS}"
  CACHE STRING "")# FORCE)
