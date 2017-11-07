# CLI of TronFerno-MCU


## Syntax

     command option1=value1 option2=value2 ... optionN=valueN;

Examples:

     send g=1 m=2 c=up;
     timer weekly=05002100++++0800- astro=-15 sun-auto=1;
	 config wlan-ssid=WLBox wlan-password=34l2cxdfa9 restart=1;


## Commands


### help

* Explanation: The help command prints a list of available commands.

* Options: none so far

* Examples:
```
     help;
```	
	
### config

* Explanation: Sets some configuration values. May be stored persistently in the MCU.

* Options: cu, rtc, baud, wlan-ssid, wlan-password, longitude, latitude, time-zone, dst, verbose, pw, set-config-password, restart

  * cu=(id|auto) - Sets the ID to be used in address field when sending commands. A receiver will listen only to commands from the central unit with the ID he was paired to.
     * id  - The ID is a 6 digit hex number, which identifies the central unit (2411) as the sender.
     * auto  - Get the ID from the air automatically.  You have to press the STOP button of your original central unit shortly after.
	  
		 
  * baud=N - baud rate of the serial interface where your terminal is connected
  
  * WLAN related options - data required to connect to WLAN
     * wlan-ssid=string  - the SSID
     * wlan-password=string - the password
  
            config wlan-ssid=1234 wlan-password=abcd restart=1;

   * restart=(0|1)  - Restarts the MCU. Should be the last option in a command line obviuosly.
   
            config wlan-password=MyNewPassword restart=1;
			 
  * geographical and time related  options, which are used to calculate times for the "astro" automatic built in to receivers. Astro closes the shutter at civil dusk.
     * longitude=N
     * latitude=N
	 * time-zone=N  - relative to GMT/UTC. Like: +1.0 or -7.0  or -6.5
	 * dst=(eu|0|1) - enable/disable daylight saving time. eu=europe. manually: 0=off, 1=on

            config longitude=-13.23452 latitude=+52.34234 time-zone=+1	dst=eu; 

     * rtc=ISO-TIME-STRING - Sets the MCUs internal real time clock.  Usually set automatically by NTP server.
         
            config 2017-12-31T23:59:59;

   * verbose=(0..5) - sets terminal output verbosity to levels 0 ... 5
   
            config verbose=5;
			
   
### send

a=(0|SenderID) hex address of the sender or receiver (add a 9 in front) or 0 for the configured CentralUnit
g=[0-7]  group number. 0 is for broadcast
m=[0-7]  group member. 0 is for broadcast all groups members
c=(up|down|stop|sun-down|sun-inst|set) command to send



### timer

daily=T T is like 0730- or 07302000 or -2000  for up 07:30 and/or down 20:00
weekly=TTTTTTT like weekly=0730-++++0900-+ (+ repeats the previous T) for up 07:30 Mon-Fri and up 09:00 Sat-Sun
astro=N This enables astro automatic. N is the offset to civil dusk in minutes. So astro=+60 closes the shutter 60 minutes after civil dusk
sun-auto=1  1 enables and 0 disables sun automatic
random=1 enables random automatic. shutter opens and closes at random times, so it looks like you are home when you are not
rtc-only=1  Update the built-in real time clock of the shutter. Don't change its programmed timers (and flags)
a, g and m: like in send command


   
   
	
