app=0
include flash_rom.mk


D ?= $(DEBUG)
ifeq ("$(D)","1")
endif

EXTRA_INCDIR   += $(EXTRA_BASE)/include

# compiler flags using during compilation of source files
CFLAGS          =  -std=c99 -Wpointer-arith -Wundef -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -mno-serialize-volatile -D__ets__ -DICACHE_FLASH
CXXFLAGS        = -Os -g -O2  -Wpointer-arith -Wundef -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -mno-serialize-volatile -D__ets__ -DICACHE_FLASH \
                  -fno-rtti -fno-exceptions -fpermissive

# linker flags used to generate the main object file

LDFLAGS         = -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static
# linker script used for the above linker step
LD_SCRIPT       = eagle.app.v6.4M.ld

ifndef SRC_BASE
SRC_BASE := .
endif

CPPFLAGS += -DSPI_FLASH_SIZE_MAP=$(size_map)

DEP_DIR := $(BUILD_BASE)/dep

# various paths from the SDK used in this project
SDK_LIBDIR      = lib
SDK_LDDIR       = ld
SDK_INCDIR      = include include/json

# select which tools to use as compiler, librarian and linker
CC      := xtensa-lx106-elf-gcc
CXX     := xtensa-lx106-elf-g++ -std=gnu++14
AR      := xtensa-lx106-elf-ar
LD      := xtensa-lx106-elf-gcc
OBJCOPY := xtensa-lx106-elf-objcopy
OBJDUMP := xtensa-lx106-elf-objdump
SIZE    := xtensa-lx106-elf-size
NM      := xtensa-lx106-elf-nm

SRC_DIR         := $(addprefix $(SRC_BASE)/,$(MODULES))
BUILD_DIRS      += $(addprefix $(BUILD_BASE)/,$(MODULES))
SDK_LIBDIR      := $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))
SDK_INCDIR      := $(addprefix -I$(SDK_BASE)/,$(SDK_INCDIR))

C_VERSION_SRC := $(SRC_BASE)/src/build_date.c
C_SRC   := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
CXX_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp $(sdir)/*.cc))
ASM_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.S))

C_VERSION_OBJ := $(patsubst %.c,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(C_VERSION_SRC)))
C_OBJ         := $(patsubst %.c,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(C_SRC)))
CXX_OBJ       := $(patsubst %.cc,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(CXX_SRC)))
ASM_OBJ       := $(patsubst %.S,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(ASM_SRC)))

SRC       := $(C_SRC) $(CXX_SRC) $(ASM_SRC)
OBJ       := $(C_OBJ) $(CXX_OBJ) $(ASM_OBJ)

LIBS            := $(addprefix -l,$(LIBS))
APP_AR          := $(addprefix $(BUILD_BASE)/,$(TARGET)_app.a)
APP_AR_IROM             := $(addprefix $(BUILD_BASE)/,$(TARGET)_appirom.a)
TARGET_OUT      := $(addprefix $(BUILD_BASE)/,$(TARGET).out)

LD_SCRIPT       := $(addprefix -T$(SDK_BASE)/$(SDK_LDDIR)/,$(LD_SCRIPT))

INCDIR          := -Isrc -I$(SRC_BASE)/src/time -I$(SRC_BASE)/src # $(addprefix -I,$(SRC_DIR))
EXTRA_INCDIR    := $(addprefix -I,$(EXTRA_INCDIR))


ifeq ($(DISTRO),1)
CPPFLAGS += -DDISTRIBUTION
endif

ifneq ("$(D)","1")
CFLAGS += -Os -g -O2
endif

ifneq ($(wildcard $(SRC_BASE)/src/sensitive/defaults.h),)
CPPFLAGS += -DHAVE_USER_DEFAULTS
endif

pri:
	echo $(OBJ)

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

define compile-objects
$(1)%.o: $(2)%.S
	$(vecho) "AS $$<"
	$(Q) $(CC) $(CPPFLAGS) -c $$< -o $$@ -MM > $$(patsubst %.o,%.d,$(DEP_DIR)/$$(subst /,-,$$@))
	$(Q) $(CC) $(CPPFLAGS) -c $$< -o $$@
$(1)%.o: $(2)%.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CFLAGS) -c $$< -MT $$@ -MM > $$(patsubst %.o,%.d,$(DEP_DIR)/$$(subst /,-,$$@))
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CFLAGS)  -c $$< -o $$@
$(1)%.o: $(2)%.cc
	$(vecho) "C+ $$<"
	$(Q) $(CXX) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CXXFLAGS) -c $$< -MT $$@ -MM > $$(patsubst %.o,%.d,$(DEP_DIR)/$$(subst /,-,$$@))
	$(Q) $(CXX) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CXXFLAGS)  -c $$< -o $$@
endef


.PHONY: all checkdirs clean flash  flashinit flasherase rebuild reflash

all: checkdirs $(TARGET_OUT) $(SUBMODULES)

$(TARGET_OUT): $(APP_AR_IROM) $(GEN_LIBS)
	$(Q) echo "flash size: " $(flash) " ldscript: " $(LD_SCRIPT)
	$(MAKE) $(C_VERSION_OBJ)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) -L$(BUILD_BASE) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group  $(LIBS) $(APP_AR_IROM) $(EXTRA_AR) $(C_VERSION_OBJ) -Wl,--end-group -o $@
	$(vecho) "Run objcopy, please wait..."
	$(Q) $(OBJCOPY) --only-section .text -O binary $@ eagle.app.v6.text.bin
	$(Q) $(OBJCOPY) --only-section .data -O binary $@ eagle.app.v6.data.bin
	$(Q) $(OBJCOPY) --only-section .rodata -O binary $@ eagle.app.v6.rodata.bin
	$(Q) $(OBJCOPY) --only-section .irom0.text -O binary $@ eagle.app.v6.irom0text.bin
	$(vecho) "objcopy done"
	$(vecho) "Run gen_appbin"
	#$(NM) --print-size --size-sort --radix=d $@
	$(SIZE) -A -d $@
	$(Q) python2 $(SDK_TOOLS)/gen_appbin.py $@ 0 $(mode) $(freqdiv) $(size_map) $(app)
	$(Q) mv eagle.app.flash.bin $(FW_BASE)/eagle.flash.bin
	$(Q) mv eagle.app.v6.irom0text.bin $(FW_BASE)/eagle.irom0text.bin
	$(Q) rm eagle.app.v6.*
	$(vecho) "No boot needed."
	$(vecho) "Generate eagle.flash.bin and eagle.irom0text.bin successully in folder $(FW_BASE)"
	$(vecho) "eagle.flash.bin-------->0x00000"
	$(vecho) "eagle.irom0text.bin---->0x10000"
	$(vecho) "Done"

$(APP_AR): $(OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cr $@ $^

checkdirs: $(BUILD_DIRS) $(FW_BASE) $(DEP_DIR)

$(BUILD_DIRS):
	$(vecho) mkdir $@
	$(Q) mkdir -p $@

$(DEP_DIR):
	$(Q) mkdir -p $@

$(FW_BASE):
	$(Q) mkdir -p $@
	$(Q) mkdir -p $@/upgrade

rebuild: clean all

reflash: rebuild flash

clean: force
	$(Q) rm -f $(APP_AR)
	$(Q) rm -f $(EXTRA_AR)
	$(Q) rm -f $(TARGET_OUT)
	$(Q) rm -rf $(BUILD_DIRS)
	$(Q) rm -rf $(DEP_DIR)
	$(Q) rm -rf $(FW_BASE)
	$(Q) rm -rf $(BUILD_BASE)/src $(BUILD_BASE)
	$(Q) mkdir -p $(DEP_DIR) # FIXME: kludge to allow "make -j"

$(C_VERSION_SRC) : force
	echo 'const char * const build_time = "'$$(date --iso-8601=seconds)'";' > $(C_VERSION_SRC)


$(eval $(call compile-objects,$(BUILD_BASE)/,$(SRC_BASE)/))

include $(wildcard $(DEP_DIR)/*.d) $(wildcard $(DEP_DIR)/.*.d)
