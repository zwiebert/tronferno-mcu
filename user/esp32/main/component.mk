#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

#COMPONENT_ADD_INCLUDEDIRS = /home/bertw/proj/mcu/tronferno-mcu/user
COMPONENT_PRIV_INCLUDEDIRS = ../..
COMPONENT_SRCDIRS += ../../main ../../misc ../../cli ../../fernotron ../../automatic ../../positions ../../setup ../../config ../../userio


ifdef DISTRO
ifeq ($(DISTRO),1)
CPPFLAGS += -DDISTRIBUTION
endif
endif

CPPFLAGS += -DMCU_ESP32

