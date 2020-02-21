set(srcs bcd.c  stof.c  sun.c)

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} INCLUDE_DIRS "./include" PRIV_INCLUDE_DIRS "./include/misc")