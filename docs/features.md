## Features

1. Like the original 2411, tronferno can...
    *  control Fernotron groups and receivers
    *  program the internal timers of single Fernotron receivers
    *  update the internal clocks and automatically changes to daylight saving time
    *  calculates civil dusk times (using geo coordinates)
    *  set end-point for shutters
    *  change rotation direction
    

2. Additionally tronferno can also...
    *  use the ID of your original programming central
        * with that, it can control all groups and receivers you have previously configured with that original central
    *  program the internal timers of whole groups
        * you can program the whole group first, and then some individual receivers with different timers after that
    *  keep track of shutter positions (or at lesat tries it)
    *  repeater function (similar to original Ferntoron 2427)
        * like with original 2427, each transmitteer must only be registered to one single repeater.
    *  connect to the users WLAN or LAN
    *  connect to MQTT server/broker
    *  provide a [web-interface](webserver.md)
    *  provide a [command line interface](CLI.md)
        * via USB or telnet  
    *  interact with a home server via [MQTT client](mqtt.md)
        *  it auto generates home server device configurations

