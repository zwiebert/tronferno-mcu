### Flash MCU

Follow the MCU specific documentation (like doc/mcu_esp8266.md).

### After flashing is done:

* optional: configure your private data if not already done above. 
  * connect a serial terminal 115200-8-N-1 with local echo enabled:
  
        gtkterm --port /dev/ttyUSB0 --speed 115200 --echo
    
  * type your commands in the terminal. Command line ends with a semicolon (;):
  ```
      help;
      config wlan-ssid="xxx" wlan-password="xxx"; 
      config latitude=xxx longitude=xxx time-zone=+1.0 dst=eu;
      config cu=auto;     (this needs a 443Mhz RF receiver connected)
  ```    
* connect a TCP terminal (like "Serial WIFI Terminal" from android play store)

     * find out which IP4 address your ESP8266 got from your WLAN accesspoint (look into the web interface of the AP).
     
     * Connect to port 7777 on this IP4 address 
     


* try to send commands from serial port terminal or TCP terminal.  RF Transmitter 443Mhz should be connected:
```
     send g=1 m=1 c=down;    (would close the shutter number 1 in group 1   
     send c=up;              (would open all shutters from all groups)
```    
     
In case you no longer have a working Fernotron central unit. You would start from scratch:
 
     config cu=80xxxx;  (just make up an address, x needs to be one of 0...9 a...f  (hex).  write it down)
    
* press the set-button on the shutter you want to be number 1 in group 1, and type quickly:
    
         send g=1 m=1 c=stop;
    
 * press the set-button on the next shutter, and type quickly (or use copy/paste)
    
         send g=1 m=2 c=stop;
    
 * ... continue until  you have added your new central unit to all shutters you want
    
 * instead of pressing the set-button on the shutter you can use the code written on the shutter motor 
    ```
        send a=9xxxxx c=set;   where xxxxx is the hexadecimal code. Set the 9 in front of it
        send a=9xxxxx c=up;    ... or send other commands using that code
    ```
 * or you could sniff out the address of a simple hand sender and sent your commands using that address. Or make up such address and add your made up address via "set" to as many shutters you like. The shutter can memorize a few of these addresses.
