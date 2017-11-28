# CLI of TronFerno-MCU

This text describe the command line interface of tronferno-mcu. The CLI can be accessed via a terminal, but is best to be acessed via a front-end, like a smart phone app or some server like FHEM.

When using a terminal, it may be required to enable local echo. Backspace key is now handled properly.
 

## Syntax

        command option1=value1 option2 option3="quoted value" ... optionN=valueN;

* Examples:

        send g=1 m=2 c=up;
        timer weekly=05002100++++0800- astro=-15 sun-auto=1;
        config wlan-ssid=WLBox wlan-password="34l2cx;dfa9" restart=1;


## Commands

Commands are help, config, send, timer


### help command

* Explanation: The help command prints a list of available commands.

* Options: config, send, timer, all

     `help config;`

    
### config command

Sets some configuration values. Most of them are stored persistently in the MCU.

Possible options are: cu, rtc, baud, wlan-ssid, wlan-password, longitude, latitude, time-zone, dst, verbose, pw, set-config-password, restart


#### cu

* cu=(id|auto) - Sets the ID to be used in address field when sending commands. A receiver will listen only to commands from the central unit with the ID he was paired to.
     * id  - The ID is a 6 digit hex number, which identifies the central unit (2411) as the sender.
     * auto  - Get the ID from the air automatically.  You have to press the STOP button of your original central unit shortly after.
      
#### baud

* baud=N - baud rate of the serial interface where your terminal is connected


#### WLAN options

Data to connect to a WLAN

* wlan-ssid=string  - the SSID
* wlan-password=string - the password
  
            config wlan-ssid=1234 wlan-password=abcd restart=1;


#### restart

* restart  - Restarts the MCU. Should be the last option in a command line obviuosly.
   
            config wlan-password=MyNewPassword restart=1;

#### Geographical and time related options.

These options  are used to calculate times for the "astro-automatic" built in to receivers. Astro closes the shutter at civil dusk. The user can define a offset time (like 10 mintutes before civil dusk).

* longitude=DEC
* latitude=DEC
* time-zone=N  - relative to GMT/UTC. Like: +1.0 or -7.0  or -6.5
* dst=(eu|0|1) - enable/disable daylight saving time. eu=europe. manually: 0=off, 1=on. If DST is on, the shutter will add 1 hour to its astro timer

        config longitude=-13.23452 latitude=+52.34234 time-zone=+1  dst=eu; 

* rtc=ISO-TIME - Sets the MCUs internal real time clock.  Usually set automatically by NTP server.
         
        config 2017-12-31T23:59:59;

* verbose=(0..5) - sets terminal output verbosity to levels 0 ... 5
   
        config verbose=5;
            
   
### send command
 
Send a plain command message via RF to the receivers. Receiver can be a shutter motor, or a light, if you have the hardware for that.

* Options are: a, g, m, c
  
#### addressing options

* a=(id|0) - the adress field of a command usually contains the ID of the sender, or sometimes the receiver
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
  * set - switch receiver to set-mode (like pressing pyhsical set button)
  
         send g=2 m=3 c=up;     shutter 3 of group 2 goes up
		 send c=down;           all shutters go down
		 send a=90abcd c=set;   motor with code label "0abcd" goes into set mode
         send c=sun-down;       all open shutters go to sun position (even if it rains)
		 
#### option SEP for setting end position

  * WARNING: the shutter can be damaged by moving the end position beyond the physical possible limit.
  * WARNING: make sure, to address a single shutter only (g=1 m=1 can address more than motor).
  * WARNING: Its best to use this with motor codes (IDs starting with 9). They will never address more than one motor.
  * WARNING: Would be best to have an additional controller handy to send a stop signal to the shutter, in cases our MCU loses power or crashes.

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

The timer command sends a message via RF to the receivers.  Such a message consists of a plain message like in send, additional RTC data and a big block with timer data, wich needs 5 seconds to be transmitted.

Except for rtc-only, all timer options should be send together by a single command line. They are all transmitted together in a monolithic data block.  So even if you just want to change the daily timer, a full data block containing weekly timer and astro timer data is send. All options ommitted will be set to their default value "disabled."

We cannot read any data from the receivers anyway, so to be able to tell a user which timer data is currently stored in a receiver, the only way is to memorize it elsewhere (in the MCU would be best, I'm working on it).  Because of this, the original central unit 2411 will not know about any timers or options set by our MCU and vice versa. All timer commands will also set the RTC of the receiver.  So the RTC of the MCU should set to the correct time, which is usally done by NTP. If NTP is not available it needs to be set by the user or the front-end (GUI).
```
   timer sun-auto astro ...;     set all options on a single command line
   timer sun-auto;                 this enables sun automatic
   timer astro;                   ... but this will disable sun automatic
   timer ...;                       ... this will disable astro
```

* Options are: a, g, m,  daily, weekly, astro, sun-auto, random, rtc-only
   
#### adressing options

* The addressing optoins a, g, m  are identical to the send option.
      
#### timer options

Eeach receiver has four built-in timers: daily, weekly, astro, random. Remember that ommited timers are disabled.

       `timer astro=-15;       will set the astro timer and disables all other timers and options`
       `timer astro=-15 daily=0600- sun-auto=1;  sets astro, daily, sun-automatic  and disables all others`
   
*  daily=T - sets the daily timer
  * T  - T is a 8 digit time string like 07302000. The four left digits are the up-time. The four on the right the down-time. A minus sign can replace 4 digits, which means the timer is cleared.

        timer daily=07302000;   up 07:30, down 20:00
        timer daily=0730-;      up 07:30, not down
        timer daily=-2000;      not up,   down 20:00


* weekly=TTTTTTT - sets a timer for each week day. week days are from left to right: monday, tuesday, wednesday, thursday, friday, saturday, sunday
  * T - Each T is a 8 digit time string like described above with daily option.  A plus sign repeats the previous T.  So you can copy the values from monday to tuesday and so on.

       `timer weekly=0730-++++0900-+;    up monday-friday at 07:30, and saturday-sunday at 09:00`

* astro=N  Sets the astro timer. The time of civil dusk for each day of the year is calcualated by geographical config settings. The shutter will then close at that time. Note: There is no automatic for open at dawn built into the receivers. Its only for closing.
  * N - time offset in minutes.  Negative number make the shutter closing earlier.

        `timer astro=-30;    closes shutter thirty minutes before civil dusk`
        `timer astro=+120;   closes shutter 2 hours after civil dusk`
		
#### rs - option to read saved timers back

The mcu stores timer commands, so it can print out the timer configuration of each receiver. It overwrites older commands by newer commands and older explicit addressed commands by newer wild-card commands:

* rs=(0|1|2) -   rs=1 will match the exacly group and member. rs=2 will also return matching wild-cards.

         timer g=2 m=3 rs=1;   returns only the timer command saved for g=2 and m=2
		 timer g=2 m=3 rs=2;   may return also the timer command saved for g=2 m=0,  g=0 m=0, g=0 m=2

#### on/off options

* random=(1|0)   enable a random timer.  To avoid the 'abandoned house' look and scare thiefs away.
* sun-auto=(1|0) enable sun automatic.  If enabled, the shutter will react to sun-down messages. Otherwise they will be ignored.

#### rtc-only
* rtc-only=(1|0)  Send only RTC data and ommit the entire timer data. Will only affect the RTC in the receiver, not any timer data.

        timer rtc-only;    updates internal RTC of the receiver(s)
	    timer rtc-only sun-auto daily=0500-;   sun-auto and daily options have no effect
		
	   
Each timer command has RTC data and will update the internal RTC of the receivers addressed by a timer command. This rtc-only option is mainly used after power loss of a shutter; to synchronize the times of all shutters; when daylight time changes.

The built-in RTC of a shutter keeps track of day time, day of month, month of year,  day of week and dst yes/no. it does not know about leap-years or years at all.



#### unknown options

   Unknown options give a warning, but will not fail the command.
   

#### common options

  Common options are accepted by all commands.
  
  
* mid=N   N is supposed to be an ID number. It will be part of the reply messages (reply@N: ...). The front-end should use this to check which reply belongs to wich command.
