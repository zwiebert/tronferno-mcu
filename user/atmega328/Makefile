RM := rm -rf
CPPFLAGS += -DMCU_ATMEGA328

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

SRC_BASE ?= ../..
BUILD_BASE ?= $(SRC_BASE)/atmega328-build
FW_BASE ?= $(BUILD_BASE)/firmware

ifeq ($(DISTRO),1)
CPPFLAGS += -DDISTRIBUTION
endif


project=fernotron
bld_dir := $(BUILD_BASE)/
modules := user/main user/atmega328
MODULES = $(modules)

SRC_DIR		:= $(addprefix $(SRC_BASE)/,$(MODULES))
BUILD_DIRS	:= $(addprefix $(BUILD_BASE)/,$(MODULES)) $(FW_BASE)

C_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
CXX_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
ASM_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.S))

C_OBJ		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(C_SRC)))
CXX_OBJ		:= $(patsubst %.cpp,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(CXX_SRC)))
ASM_OBJ	    	:= $(patsubst %.S,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(ASM_SRC)))

SRC		:= $(C_SRC) $(CXX_SRC) $(ASM_SRC)
OBJ		:= $(C_OBJ) $(CXX_OBJ) $(ASM_OBJ)

time_modules := user/time
TIME_ASM_SRC := $(wildcard $(SRC_BASE)/user/time/*.S)
TIME_C_SRC := $(wildcard $(SRC_BASE)/user/time/*.c)

TIME_C_OBJ		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(TIME_C_SRC)))
TIME_ASM_OBJ	    := $(patsubst %.S,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(TIME_ASM_SRC)))

TIME_OBJ := $(TIME_C_OBJ) $(TIME_ASM_OBJ)


OUTPUT_FILE_PATH += $(FW_BASE)/fernotron.elf

OUTPUT_FILE_PATH_AS_ARGS += $(FW_BASE)/fernotron.elf


#elf_out = $(bld_dir)$(project).elf
elf_out = $(FW_BASE)/fernotron.elf
hex_out = $(FW_BASE)/fernotron.hex
eep_out =  $(FW_BASE)/fernotron.eep
srec_out =  $(FW_BASE)/fernotron.rec
lss_out =   $(FW_BASE)/fernotron.lss
map_out =  $(FW_BASE)/fernotron.map

DEP_DIR := $(BUILD_BASE)/dep

pri:
	echo $(TIME_OBJ)

# AVR32/GNU C Compiler

AVR_INCLUDE:= -I/usr/lib/avr/include -I./user/time
PROJ_INCLUDE:= -I$(SRC_BASE)/user
 
CC = avr-gcc
ASMFLAGS := -c -Wall -mmcu=atmega328p 
LDFLAGS := -L $(bld_dir)
LIBS := -ltime
AR = avr-ar


CPPFLAGS += -DF_CPU=16000000UL -DAVR_TIME -DAVR -DFER_RECEIVER -DFER_TRANSMITTER  -DICACHE_FLASH_ATTR=  -I"/home/bertw/avr/Packs/atmel/ATmega_DFP/1.2.132/include"  $(PROJ_INCLUDE) $(AVR_INCLUDE)
CFLAGS =  -c -x c -funsigned-char -funsigned-bitfields  -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -mmcu=atmega328p  -B "/home/bertw/avr/Packs/atmel/ATmega_DFP/1.2.132/gcc/dev/atmega328p" -gdwarf-2 -std=c99

D ?= $(DEBUG)
ifeq ("$(D)","1")
CPPFLAGS += -DDEBUG -DDEBUGGER -DDB_SELF_TEST
CFLAGS +=  -Og -g3
else
CPPFLAGS += -DDB_SHORT -DNO_PRECOND -DNO_POSTCOND
CFLAGS +=  -O3

endif

GCC_VER = 4.9.2


define compile-objects
$(1)%.o: $(2)%.S
	$(vecho) "AS $$<"
	$(Q) $(CC) $(ASMFLAGS) -o $$@ $$< 
$(1)%.o: $(2)%.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CFLAGS) -c $$< -MT $$@ -MM > $$(patsubst %.o,%.d,$(DEP_DIR)/$$(subst /,-,$$@))
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CFLAGS)  -c $$< -o $$@
$(1)%.o: $(2)%.cpp
	$(vecho) "C+ $$<"
	$(Q) $(CXX) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CXXFLAGS)  -c $$< -o $$@
endef


# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler



# Add inputs and outputs from these tool invocations to the build variables 

$(bld_dir)libtime.a: $(bld_dir) $(TIME_OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $(TIME_OBJ)


# All Target

.PHONY: all clean flash rebuild reflash

all: $(OUTPUT_FILE_PATH)


$(BUILD_DIRS):
	mkdir -p $(foreach dir,$(modules) $(time_modules),$(addprefix $(bld_dir)/,$(dir))) $(DEP_DIR) $(FW_BASE)


$(OUTPUT_FILE_PATH): $(BUILD_DIRS) $(OBJ) $(bld_dir)libtime.a
	$(vecho) "LD builds $(elf_out)"
	$(Q) $(QUOTE)avr-gcc$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJ) $(LDFLAGS) $(LIBS) -Wl,-Map="$(map_out)" -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega328p  -B "/home/bertw/avr/Packs/atmel/ATmega_DFP/1.2.132/gcc/dev/atmega328p"  -Wl,-gc-sections  
	$(Q) "avr-objcopy" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "$(elf_out)" "$(hex_out)"
	$(Q) "avr-objcopy" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "$(elf_out)" "$(eep_out)" || exit 0
	$(Q) "avr-objdump" -h -S "$(elf_out)" > "$(lss_out)"
	$(Q) "avr-objcopy" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "$(elf_out)" "$(srec_out)"
	$(Q) "avr-size" "$(elf_out)"
	$(Q) echo; ls -l $(FW_BASE)/$(project).*


COM := /dev/ttyACM0

flash: all
	$(Q) avrdude  -c avrisp2 -p m328p -P $(COM) -b 115200 -U "flash:w:$(hex_out):i"

eeprom: all
	$(Q) avrdude  -c avrisp2 -p m328p -P $(COM) -b 115200 -U "eeprom:w:$(eep_out):i"


# Other Targets
clean:
	-$(RM) $(bld_dir) $(DEP_DIR) 
	$(Q) mkdir -p $(DEP_DIR) # FIXME: kludge to allow "make -j" 



rebuild : clean all

reflash : rebuild flash


$(eval $(call compile-objects,$(BUILD_BASE)/,$(SRC_BASE)/))

include $(wildcard $(DEP_DIR)/*.d) $(wildcard $(DEP_DIR)/.*.d)

