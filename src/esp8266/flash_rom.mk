
ifeq ($(SPI_SPEED), 26.7)
freqdiv = 1
flashimageoptions = -ff 26m
else
ifeq ($(SPI_SPEED), 20)
freqdiv = 2
flashimageoptions = -ff 20m
else
ifeq ($(SPI_SPEED), 80)
freqdiv = 15
flashimageoptions = -ff 80m
else
freqdiv = 0
flashimageoptions = -ff 40m
endif
endif
endif

ifeq ($(SPI_MODE), QOUT)
mode = 1
flashimageoptions += -fm qout
else
ifeq ($(SPI_MODE), DIO)
mode = 2
flashimageoptions += -fm dio
else
ifeq ($(SPI_MODE), DOUT)
mode = 3
flashimageoptions += -fm dout
else
mode = 0
flashimageoptions += -fm qio
endif
endif
endif

addr = 0x01000

ifeq ($(SPI_SIZE_MAP), 1)
size_map = 1
flash = 256
flashimageoptions += -fs 256KB
else
ifeq ($(SPI_SIZE_MAP), 2)
size_map = 2
flash = 1024
flashimageoptions += -fs 1MB
else
ifeq ($(SPI_SIZE_MAP), 3)
size_map = 3
flash = 2048
flashimageoptions += -fs 2MB
else
ifeq ($(SPI_SIZE_MAP), 4)
size_map = 4
flash = 4096
flashimageoptions += -fs 4MB
else
ifeq ($(SPI_SIZE_MAP), 5)
size_map = 5
flash = 2048
flashimageoptions += -fs 2MB-c1
else
ifeq ($(SPI_SIZE_MAP), 6)
size_map = 6
flash = 4096
flashimageoptions += -fs 4MB-c1
else
size_map = 0
flash = 512
flashimageoptions += -fs 512KB
endif
endif
endif
endif
endif
endif

# ===============================================================
# From http://bbs.espressif.com/viewtopic.php?f=10&t=305
# master-device-key.bin is only need if using espressive services
# master_device_key.bin 0x3e000 is not used , write blank
# See 2A-ESP8266__IOT_SDK_User_Manual__EN_v1.1.0.pdf
# http://bbs.espressif.com/download/file.php?id=532
#
# System parameter area is the last 16KB of flash
# 512KB flash - system parameter area starts from 0x7C000
# 	download blank.bin to 0x7E000 as initialization.
# 1024KB flash - system parameter area starts from 0xFC000
# 	download blank.bin to 0xFE000 as initialization.
# 2048KB flash - system parameter area starts from 0x1FC000
# 	download blank.bin to 0x1FE000 as initialization.
# 4096KB flash - system parameter area starts from 0x3FC000
# 	download blank.bin to 0x3FE000 as initialization.
# ===============================================================

# FLASH SIZE


ifeq ($(flash), 512)
sparea = 0x7C000
bbarea = 0x7E000
else
ifeq ($(flash), 1024)
sparea = 0xFC000
bbarea = 0xFE000
else
ifeq ($(flash), 2048)
sparea = 0x1FC000
bbarea = 0x1FE000
else
ifeq ($(flash), 4096)
sparea = 0x3FC000
bbarea = 0x3FE000
endif
endif
endif
endif


.PHONY: flash flashinit flasherase
flash: all
	$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) 0x00000 $(FW_BASE)/eagle.flash.bin 0x10000 $(FW_BASE)/eagle.irom0text.bin
flasherase:
	$(vecho) "Flash erase. May be followed by: make flashinit"
	$(ESPTOOL) -p $(ESPPORT) erase_flash
flashinit:
	$(vecho) "Flash init data default and blank data."
	$(ESPTOOL) -p $(ESPPORT) write_flash $(flashimageoptions) $(sparea) $(SDK_BASE)/bin/esp_init_data_default_v08.bin $(bbarea) $(SDK_BASE)/bin/blank.bin
