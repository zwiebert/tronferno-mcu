## How you make it work



### First step: flashing firmware and set up network connection to access web-interface


 1. Have the [hardware](hardware.md) ready (esp32 board and RF module(s))
    * You can do all the wiring now or later.
    * Make sure, that all the wiring is solid and be suspicious of cheap breadboards.
    *  Connect wires to the correct pins for the configured GPIOs
 1. [Download](https://codeload.github.com/zwiebert/tronferno-mcu-bin/zip/master) and [flash](starter_flash-de.md) the esp32-firmware
 1. Set up [network connection](network.md)
    * its easy to just use the menu-tool. it can not only flash firmware, but also set up the WLAN connection
    * if you did not use menu-tool you can connect to WLAN AP web-interface for setting up the network connection there
    * you can also use a terminal on USB to set up network connection with CLI
 2. Open the [web-interface](webserver.md)
    * the web-interface is self-documenting using mouse-over tool-tips. this has all the information you need and its the most up-to-date

You can now do all further set up work from inside the web interface.

### Second step: Make it control your Fernotron receivers
 1. set up the ESP32-GPIOs where your RF module(s) are connected to
    * one GPIO for RF data transmitter line
    * one GPIO for RF data receiver line(if you want that)
    * one GPIO for user button (to authenticate adjusting shutter end points)
    * if using CC1101 transceiver module: four GPIOs for its SPI interface
  
 4. Scan the ID of your original programming central.
      * You can also find the ID on a label located behind the batteries, and can enter it manually

 tronferno can now control all your Fernotron receivers registered to the original programming central

### Third step: Copy the data from your original programming central

  You have to enter the data manually, like you would do when replacing the original programming central with another one.

 1. set up number of groups and receivers
 2. set up names of groups and receivers
 1. enter geo coordinates for your place (for astro timers)
 3. set up internal timers
    * you basically program all the timer again
    * the astro times may slightly differ from the original
    * there are alternative astro time tables (option astro-correction)

tronferno will now show only the groups and receiver which are used. it will now only calculate and report positions for these groups/receivers, which saves resources.
  
### Optional step: Set up position tracking
   
  1. measure the run times of each shutter
  2. register existing Fernotron transmitters
      * give each transmitter a name
      * tell tronferno to which receivers a transmitter is registered
  
tronferno can not show the current positions and move the shutter to a wanted position.

### Optional step: Home Server Integration   
 1. Integrate with home server
    * Set up [MQTT client](mqtt.md)
    * (Auto)-create device configuration for home server
    * For fhem, there is also an optional module [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem) which can be used instead of MQTT. It communicates with FHEM via USB or IP/TCP. This module was written before tronferno-mcu had MQTT and a web-app. Its no longer that useful.
 2.  Backup your settings
 3.  Update firmware to get bug-fixes and new features.

 if your home-server supports it, you can now control your shutters using voice commands (with alexa)
