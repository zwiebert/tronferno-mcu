# CLI of TronFerno-MCU

This text describes the command line interface (CLI) of tronferno-mcu. The CLI can be accessed via a terminal or via front-ends like a smart phone app or an FHEM module..

When using a terminal, local echo should be enabled, so you can see what you type. Backspace key is working.
 

## Syntax

        command option1=value1 option2 option3="quoted value" ... optionN=valueN;

* Examples:

        send g=1 m=2 c=up;
        timer weekly=05002100++++0800- astro=-15 sun-auto=1;
        config wlan-ssid=WLBox wlan-password="34l2cx;dfa9" restart=1;


## Commands

Commands are help, config, send, timer, mcu, pair


### help command

* Explanation: The help command prints a list of available commands.

* Options: config, send, timer, all

     `help config;`

    
### config command

Sets or gets some configuration values. Most of them are stored persistently in the MCU.

Possible options are: cu, rtc, baud, wlan-ssid, wlan-password, longitude, latitude, time-zone, dst, verbose, pw, set-config-password, restart

To set a value: config option=value
To get a value: config option=?  (not supported by all options)

#### cu

* cu=(id|auto|?) - Sets the ID to be used in address field when sending commands. A receiver will listen only to commands from the central unit with the ID he was paired to.
     * id  - The ID is a 6 digit hex number, which identifies the central unit (2411) as the sender.
     * auto  - Get the ID from the air automatically.  You have to press the STOP button of your original central unit shortly after.
	 * print current value
      
#### baud

* baud=(N|?) - baud rate of the serial interface where your terminal is connected



#### WLAN options

Data to connect to a WLAN

* wlan-ssid=(string|?)  - Wlan SSID
* wlan-password=string - Wlan password
  
            config wlan-ssid=1234 wlan-password=abcd restart=1;


#### restart

* restart  - Restarts the MCU. Should be the last option in a command line obviously.
   
            config wlan-password=MyNewPassword restart=1;

#### Geographical and time related options.

These options  are used to calculate times for the "astro-automatic" built in to receivers. Astro closes the shutter at civil dusk. The user can define a offset time (like 10 minutes before civil dusk).

* longitude=(DEG|?)
* latitude=(DEG|?)
* time-zone=(N|?)  - relative to GMT/UTC. Like: +1.0 or -7.0  or -6.5
* dst=(eu|0|1|?) - enable/disable daylight saving time. eu=europe. manually: 0=off, 1=on. If DST is on, the shutter will add 1 hour to its astro timer

        config longitude=+13.23452 latitude=+52.34234 time-zone=+1  dst=eu; 

* rtc=(ISO-TIME|?) - Sets the MCUs internal real time clock.  Usually set automatically by NTP server.
         
        config 2017-12-31T23:59:59;

* verbose=(0..5|?) - sets terminal output verbosity to levels 0 ... 5
   
        config verbose=5;
            
#### additional options

* gpioN=(i|p|o|0|1|d|?) Set gpio pin as input (i,p) or output (o,0,1) or use default
  This options configures some GPIO pins of the MCU which can then be used freely for your own purposes. Like the other config options, its persistent.

  Examples:
    config gpio12=i;           sets gpio-12 as input pin. can then be read by CLI command: mcu gpio12=?
    config gpio13=1;           sets gpio-13 as output pin and the initial output level (after each reboot) is On.
    config gpio14=d restart=1; sets gpio-14 back to its default (it will be left alone). works only after reboot.
    config gpio15=?;           returns current configuration of gpio15

### send command
 
Send a plain command message via RF to the receivers. Receiver can be a shutter motor, or a light, if you have the hardware for that.

* Options are: a, g, m, c
  
#### addressing options

* a=(id|0) - the address field of a command usually contains the ID of the sender, or sometimes the receiver
  * id - 6 digit hex ID of sender or receiver
  * 0 - use internally stored central unit ID set by config cu. (default)
        
* g=(0|[1-7])  - group number
  * 0 - broadcast to all groups  (default)
  * [1-7] send to a specific group. Groups are numbered from 1 to 7.
        
* m=(0|[1-7])  - group-member number
  * 0 - broadcast to all members (default)
  * [1-7] sent to specific member. Members are numbered from 1 to 7.
        
#### action command option
* c=command - specify the action command to be send
  * up - shutter up
  * down - shutter down
  * stop - shutter stop, (in set mode: add/remove sender from receiver)
  * sun-inst - defines current shutter position as sun-position
  * sun-down - moves shutter down to predefined sun-position
  * set - switch receiver to set-mode (like pressing physical set button)
  
         send g=2 m=3 c=up;     shutter 3 of group 2 goes up
		 send c=down;           all shutters go down
		 send a=90abcd c=set;   motor with code label "0abcd" goes into set mode
         send c=sun-down;       all open shutters go to sun position (even if it rains)
		 
#### option SEP for setting end position

  * WARNING: the shutter can be damaged by moving the end position beyond the physical possible limit.
  * WARNING: make sure, to address a single shutter only (note: something like "g=1 m=1" can address more than one motor).
  * WARNING: Its best to use this with motor codes (IDs starting with 9). They will never address more than one motor.
  * WARNING: Would be best if you have an additional controller handy to send a stop signal to the shutter, in case something goes wrong (power loss, etc) while adjusting.

By adding the SEP option to c=up or c=down, the set-end-position-mode is entered.  In this mode you hold a hardware button to move the shutter in the direction you have specified with c=up/down.
If you release the button, the shutter position will be saved as the new end position for that direction.
You can hold down the button several time until you are satisfied with the new end position.  If you don't press the button for 30 seconds, the set-end-position-mode will end by timeout.
  
  On ESP8266 we use GPIO0 as hardware button. Its connected to the "Flash" button on some boards.


       send g=2 m=3 c=up SEP;    enter SEP mode.  You can set upper end position with hardware button
	   send SEP;                 disable SEP mode (if still enabled)
	   send g=2 m=3 c=down SEP;  enter SEP mode.  You can set lower end position with hardware button
	   send SEP;                 disable SEP mode
	   
 * WARNING: When you press the hardware button, the up or down command is sent once. When you release the button, a stop command is sent. If that stop command is not received, the shutter will continue moving. Try pressing and releasing the button again or use another controller to send stop. Or cut the power to the shutter. Otherwise it may be damaged.
       
### timer command

The timer command sends a message via RF to the receivers.  Such a message consists of a plain message like in send, additional RTC data and a big block with timer data, which needs 5 seconds to be transmitted.

Except for rtc-only, all timer options should be send together by a single command line. They are all transmitted together in a monolithic data block.  So even if you just want to change the daily timer, a full data block containing weekly timer and astro timer data is send. All options omitted will be set to their default value "disabled."

We cannot read any data from the receivers anyway, so to be able to tell a user which timer data is currently stored in a receiver, the only way is to memorize it elsewhere (in the MCU would be best, I'm working on it).  Because of this, the original central unit 2411 will not know about any timers or options set by our MCU and vice versa. All timer commands will also set the RTC of the receiver.  So the RTC of the MCU should set to the correct time, which is usually done by NTP. If NTP is not available it needs to be set by the user or the front-end (GUI).
```
   timer sun-auto astro ...;     set all options on a single command line
   timer sun-auto;                 this enables sun automatic
   timer astro;                   ... but this will disable sun automatic
   timer ...;                       ... this will disable astro
```

* Options are: a, g, m,  daily, weekly, astro, sun-auto, random, rtc-only
   
#### addressing options

* The addressing options a, g, m  are identical to the send option.
      
#### timer options

Each receiver has four built-in timers: daily, weekly, astro, random. Remember that omitted timers are disabled.

       `timer astro=-15;       will set the astro timer and disables all other timers and options`
       `timer astro=-15 daily=0600- sun-auto=1;  sets astro, daily, sun-automatic  and disables all others`
   
*  daily=T - sets the daily timer
  * T  - T is a 8 digit time string like 07302000. The four left digits are the up-time. The four on the right the down-time. A minus sign can replace 4 digits, which means the timer is cleared.

        timer daily=07302000;   up 07:30, down 20:00
        timer daily=0730-;      up 07:30, not down
        timer daily=-2000;      not up,   down 20:00


* weekly=TTTTTTT - sets a timer for each week day. week days are from left to right: Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
  * T - Each T is a 8 digit time string like described above with daily option.  A plus sign repeats the previous T.  So you can copy the values from Monday to Tuesday and so on.

       `timer weekly=0730-++++0900-+;    up Monday-Friday at 07:30, and Saturday-Sunday at 09:00`

* astro=N  Sets the astro timer. The time of civil dusk for each day of the year is calculated by geographical config settings. The shutter will then close at that time. Note: There is no automatic for open at dawn built into the receivers. Its only for closing.
  * N - time offset in minutes.  Negative number make the shutter closing earlier.

        `timer astro=-30;    closes shutter thirty minutes before civil dusk`
        `timer astro=+120;   closes shutter 2 hours after civil dusk`
		
#### rs - option to read saved timers back

The mcu stores timer commands, so it can print out the timer configuration of each receiver. It overwrites older commands by newer commands and older explicit addressed commands by newer wild-card commands:

* rs=(0|1|2) -   rs=1 will match the exactly group and member. rs=2 will also return matching wild-cards.

         timer g=2 m=3 rs=1;   returns only the timer command saved for g=2 and m=2
		 timer g=2 m=3 rs=2;   may return also the timer command saved for g=2 m=0,  g=0 m=0, g=0 m=2

#### on/off options

* random=(1|0)   enable a random timer.  To avoid the 'abandoned house' look and scare burglars away.
* sun-auto=(1|0) enable sun automatic.  If enabled, the shutter will react to sun-down messages. Otherwise they will be ignored.

#### rtc-only, rtc
* rtc-only=(1|0)  Send only RTC data and leave out the entire timer data. Will only affect the RTC in the receiver, not any timer data.

        timer rtc-only;    updates internal RTC of the receiver(s)
	    timer rtc-only sun-auto daily=0500-;   sun-auto and daily options have no effect

* rtc=ISO_TIME   you may provide your own time instead of current MCU/NTP time to update internal clock

        timer g=2 m=1 astro=0 rtc=2019-01-20T11:59:00;   set astro timer and use providet time to set motor-internal RTC
	    
		
	   
Each timer command has RTC data and will update the internal RTC of the receivers addressed by a timer command. This rtc-only option is mainly used after power loss of a shutter; to synchronize the times of all shutters; when daylight time changes.

The built-in RTC of a shutter keeps track of day time, day of month, month of year,  day of week and dst yes/no. it does not know about leap-years or years at all.



#### unknown options

   Unknown options give a warning, but will not fail the command.
   

#### common options

  Common options are accepted by all commands.
  
  
* mid=N   N is supposed to be an ID number. It will be part of the reply messages (reply@N: ...). The front-end should use this to check which reply belongs to which command.


### mcu command

#### set or read MCU pins

*  gpioN=(0|1|t|?) clear, set, toggle or read GPIO pin N

   Before using this command, configure the pin via "config gpioN"  once (it will survive reboots).

   Examples:

     config gpio12=0 gpio13=p;    configure gpio12 as output and gpio13 as input with pull-up resistor (configuration will survive reboots)
     mcu gpio12=t gpio13=?;      toggle gpio12 output pin and read gpio13 input pin

* print=(rtc|reset-info)    prints some information: rtc: current real time clock. reset-info: MCU reset info (to find out why and where the firmware has crashed) 


### pair command (experimental state)

 currently its used to pair a shutter specified by g/m parameter to a controller specified by a parameter. Commands received by a paired controller will modify the tracked status of the paired g/m and generate status change messages for it.

not fully implemented yet: to pair input and output pins to g/m. So a shutter can be controlled by output pins for up/down. And an up/down/stop switch at the pins can generate up/down/stop commands.

  *a=(?|ID) 0  controller to pair. '?' starts auto-scan
  *g=[0-7]   0  group number
  *m=[0-7]   0  group member number
  *gpoutN=(up|down|switch)   
  *gpinN=(up|down|stop|rain|toggle)
  *c=(pair|unpair|read)


