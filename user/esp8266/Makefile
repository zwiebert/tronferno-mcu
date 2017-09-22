# Main settings includes
include	 ./user/esp8266/settings.mk

# Individual project settings (Optional)
#BOOT		= new
#APP		= 1
#SPI_SPEED	= 40
#SPI_MODE	= QIO
SPI_SIZE_MAP	= 4
ESPPORT		= COM11
#ESPBAUD		= 115200

#V=1
# Basic project settings
MODULES	=   user/main user/esp8266 user/esp8266/driver user/time
LIBS	= c gcc hal phy pp net80211 lwip wpa main crypto
# Root includes
#include	C:/Espressif/examples/ESP8266/common_nonos.mk
include ./user/esp8266/common.mk
