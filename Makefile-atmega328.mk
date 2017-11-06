RM := rm -rf

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif




project=fernotron

build_version := lean
src_dir=./
bld_dir=$(build_version)/


modules := user/main user/atmega328
proj_asrc := $(foreach dir,$(src_dir)$(modules),$(wildcard $(dir)/*.S))
proj_csrc := $(foreach dir,$(src_dir)$(modules),$(wildcard $(dir)/*.c))
proj_objs := $(addprefix $(bld_dir),$(patsubst %c,%o,$(proj_csrc))) $(addprefix $(bld_dir),$(patsubst %S,%o,$(proj_asrc)))

time_modules := user/time
time_asrc := $(wildcard $(src_dir)user/time/*.S)
time_csrc := $(wildcard $(src_dir)user/time/*.c)
time_objs := $(addprefix $(bld_dir),$(patsubst %c,%o,$(time_csrc))) $(addprefix $(bld_dir),$(patsubst %S,%o,$(time_asrc)))


OUTPUT_FILE_PATH += $(bld_dir)fernotron.elf

OUTPUT_FILE_PATH_AS_ARGS += $(bld_dir)fernotron.elf


elf_out = $(bld_dir)$(project).elf
hex_out = $(patsubst %elf,%hex,$(elf_out))
eep_out = $(patsubst %elf,%eep,$(elf_out))
srec_out = $(patsubst %elf,%srec,$(elf_out))
lss_out = $(patsubst %elf,%lss,$(elf_out))
map_out = $(patsubst %elf,%map,$(elf_out))


pri:
	echo $(time_objs)

# AVR32/GNU C Compiler

AVR_INCLUDE:= -I/usr/lib/avr/include -I./user/time
PROJ_INCLUDE:= -I$(src_dir)user
 
CC = avr-gcc
ASMFLAGS := -c -Wall -mmcu=atmega328p 
LDFLAGS := -L $(bld_dir)
LIBS := -ltime
AR = avr-ar


CPPFLAGS = -DF_CPU=16000000UL -DAVR_TIME -DAVR -DFER_RECEIVER -DFER_TRANSMITTER  -DICACHE_FLASH_ATTR=  -I"/home/bertw/avr/Packs/atmel/ATmega_DFP/1.2.132/include"  $(PROJ_INCLUDE) $(AVR_INCLUDE)
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


$(bld_dir)%.o: $(src_dir)%.c
	$(vecho) "CC $<"
	$(Q) $(CC) $(CPPFLAGS) $(CFLAGS) -o "$@" "$<" 


$(bld_dir)%.o: $(src_dir)%.S
	$(vecho) "AS $<"
	$(Q) $(CC) $(ASMFLAGS) -o "$@" "$<" 

# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler



# Add inputs and outputs from these tool invocations to the build variables 

$(bld_dir)libtime.a: $(bld_dir) $(time_objs)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $(time_objs)


# All Target

.PHONY: all clean flash rebuild reflash

all: $(OUTPUT_FILE_PATH)


$(bld_dir):
	mkdir -p $(foreach dir,$(modules) $(time_modules),$(addprefix $(build_version)/,$(dir)))


$(OUTPUT_FILE_PATH): $(bld_dir) $(proj_objs) $(bld_dir)libtime.a
	$(vecho) "LD builds $(elf_out)"
	$(Q) $(QUOTE)avr-gcc$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(proj_objs) $(LDFLAGS) $(LIBS) -Wl,-Map="$(map_out)" -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega328p  -B "/home/bertw/avr/Packs/atmel/ATmega_DFP/1.2.132/gcc/dev/atmega328p"  -Wl,-gc-sections  
	$(Q) "avr-objcopy" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "$(elf_out)" "$(hex_out)"
	$(Q) "avr-objcopy" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "$(elf_out)" "$(eep_out)" || exit 0
	$(Q) "avr-objdump" -h -S "$(elf_out)" > "$(lss_out)"
	$(Q) "avr-objcopy" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "$(elf_out)" "$(srec_out)"
	$(Q) "avr-size" "$(elf_out)"
	$(Q) echo; ls -l $(bld_dir)$(project).*
	
	
COM := /dev/ttyACM0
	
flash: all
	$(Q) avrdude  -c avrisp2 -p m328p -P $(COM) -b 115200 -U "flash:w:$(hex_out):i"

eeprom: all
	$(Q) avrdude  -c avrisp2 -p m328p -P $(COM) -b 115200 -U "eeprom:w:$(eep_out):i"


# Other Targets
clean:
	-$(RM) $(build_version)
	

rebuild : clean all

reflash : rebuild flash


	
