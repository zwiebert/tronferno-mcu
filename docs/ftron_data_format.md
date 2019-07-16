```
Warning: This document is meant to provide information for developers.
Warning: My understanding comes from reverse engineering the data sent by my Fernotron devices.
Warning: Do not consider it reliable information.
Warning: It's just guessing, try and error. I may also have made mistakes in writing things down.   
Warning: There is risk of damaging things when using devices not made by the original equipment manufacturer.
Warning: Use any information in this file only at your own risk.
```



Basics
======

Fernotron works uni-directional:

A controller device transmits a message on 434 MHz and all receiving devices listen and filter out the messages addressed to them, or the group they belong to. They cannot acknowledge to the sender, that they received the data.  They can only give feedback to the user itself, by moving the shutter a bit when they have succesfully received a command which not involves moving the shutter directly.


Each device has an unique ID. On motors this ID is usually printed on the motor itself and on some sticker on the cable. That ID can be used to access the motor without having to press the physical set button, which may not easy accessible.

Controller devices like 2411 or 2431 have an ID too, but its not printed on anywhere. They can be only seen by looking at the messages they send.


Each message has a single address field.  This address can contain a Sender ID (like From:) *or* an Receiver ID (like To:).

A receiver will listen to all addresses which are equal to its own receiver ID. Because the IDs are supposed to unique, all other receivers will ignore these messages.


Additionally, each receiver keeps a list in its persistent memory, containing sender IDs he also will listen to.  If you activate the set function on a receiver and send a "stop" message around from a controller, the ID in that message address field will be added (or removed) from that list.  In case of sent by the central unit (2411), the receiver will also memorize the group and member number (which are also part of the message).


Message "Envelope"
==================

Simple messages like up/down/stop are are encoded into 5 bytes.  If you counted 6 bytes in log output ... yes its really 6, but the 6th contains just a check-sum of that 5 bytes. You may have looked at the data, which actually is sent via 434 MHz, and it looks much more than just 6 bytes ... ok, there really are 12 words of 10bit. The lower 8bit of each word pair is identical to one of the 6 bytes  (word 0/1 = byte 0, word 2/3 = byte 1, ...  word 10/11 = byte 6). The 2 extra bit in each word contain bit parity and indicate if a word is odd or even-numbered.  ... so its really only 5 data bytes. These contain all the addressing info:
```
 bytes 0,1,2: this is the address-field which contains the 3 byte device ID
 byte 3 high nibble: A counter which may increment with each button press
 byte 3 low nibble: member number (or type of sender if not sent by central unit)
 byte 4 high nibble: group number
 (byte 4 low  nibble: used for message content)
```

Explaining Group and Member number
==================================
The central unit uses group/member numbers to control shutters independently. The central unit 2411 lets you configure 7 groups each having up to 7 members.  You can assign multiple motors to a single Group/Member combo, but not one motor to multiple Group/Member combinations.  So group=1/member=1 could address all windows in a room if you want. But group=1/member=1 and group=2/member=1 can never address the same motor.


Explaining the Device ID
========================

* 3-byte Device ID, There are different kind of IDs. The kind is encoded in the first nibble (4 bit)  of the ID:  plain-sender: 0x1, sun-sensor: 0x2, central-unit=0x8, receiver=0x9.

   * ID of a receiver (9xxxx):  A motor has a designated 5 digit hex code number. This number is printed on the motor itself and on the cable-sticker. 5 digits are 5 nibbles or 2.5 bytes, not 3.  Prefix these 2.5 bytes with number 9. This will be the Device ID of the Receiver.  If, for example, your original 2411 central unit gets destroyed, you need to press the set button of a motor to pair it to your new 2411. Or you can add that code into your new 2411 and send the "press the button" command via RF to that motor.

   * ID of central unit (8xxxx). Its not printed out anywhere, because there is no need to know it, when using the original hardware.

  Example: To pair a motor with a central unit, you first press the set button on the motor. Now you send a "Stop" command to Number 3 of Group 2.  The 5-byte command sent will contain:

    * ID of the central Unit, which will the motor store in its local persistent memory.
    * The group number (1...7) and a group-member number (1...7) which will also be stored. 

   Each motor can store only one central unit ID, group and member number. The motor will now listen individually to commands containing that ID and Group/Member number combination. To make more than one motor follow this combo (e.g. large windows with 2 motors), these motors are paired with the same group/member number.

 
   To make multiple motors with different group or member number  listen to a single command, the number 0 acts as a wildcard for group and/or member.
   Group=1 and member= 0 targets all members of group 1. Other possible combinations:  group=0 member=0 (all)  group=0 member=1 (not so useful, but it works).


  * ID of sun-sensor (9xxxx):  Each motor can store one sun-sensor ID in its memory. All motors which stored that ID will listen to the commands containing that ID. So one sun-sensor can control multiple motors, but one motor can not controlled by multiple sun-sensors.


  * ID of a plain sender (1xxxx).  Each motor can store *multiple* plain senders and will listen to commands containing one of these IDs.  Like with sun-sensor, multiple motors can be controlled by one plain sender, because you can add its ID to as many motors you want.  But unlike sun-sensor, you can also have multiple plain senders each controlling the same motor. This makes them very versatile:  One plain sender could glued to the window frame and controls just the shutter on this  window. Another one could be glued near the door frame, and control the shutters on all the windows in a room.  Another one could control the entire floor, or even  the entire house.


Explaining the Counter
=======================

A nibble counted up with each button press. Zero is skipped: ...a,b,c,d,e,f,1,2,3...

When pressing a button, a message is sent not only once, but will be repeated as long the button is pressed.  Repeating a message is done to increase the chance the receiver gets the message at least once.

So the receiver could identify and disregard duplicates because the counter stays the same.  The counter acts like a message ID.

Unlike the other buttons, holding down the stop button on a 2411 increments the counter for each repeat. So each 'Stop' message is not a duplicate but an original. So stop will never be disregarded.



Message Content
===============

Now for the message content. Simple messages can fit into the nibble we have left at the end of the 5 bytes mentioned  above:

byte 4 low nibble: command code number. e.g. stop (encoded as number 3), up (4), down (5), ...

To set the RTC and the built-in timers longer messages are required.
```
message up/down/stop/etc                 ; 4.5 + 0.5 bytes
message timer rtc-data                   ; 4.5 + 0.5 + 8 bytes
message timer rtc-data  timer-data extra ; 4.5 + 0.5 + 8 + (8*16) + 8 bytes 
```

How its transmitted
===================
The checksum is added to the 5 byte command and  6th byte is added containing a checksum of the 5 byte command.  These 6 bytes are made into 12 bytes by doubling each byte. So byte 0 becomes byte 0 and 1 ... and byte 5 becomes byte 10 and byte 11.  Then 8bit bytes are extended to 10 bit words.  Bits 0-7 still contain the data byte. Bit 8 is 0 for even words (0, 2, 4 .. 10) and 1 for odd words (1, 3, 5 .. 11).  Bit 9 is for bit parity (so each word has even number of 1 and 0 bits).

The 12 words will be send with a pause between each word. This pause its called stop bit in my code. Its like a normal bit with the negative wave extend for the duration of 2 normal bits. There is also the preamble which starts every transmission.

When setting up the built-in RTC and timers in the motor, the data words containing time and timer data are just appended.

So the number of sent words goes from 12 (plain command) to 30 (+ RTC data) up to 336 (+ Timer data).  It takes 5 seconds to transmit 336 words.


The bits are modulated by the timing the carrier is switched on and off.   Short-on/long-off means the bit is set (1).  Long-on/short-off means the bit is clear (0).



Back to the Bytes
=================

Now lets look at all the bytes inside a timer message. Its organized in lines. Each line ends with a checksum, except the very last one, which seems to be special.

It starts with a plain 5-byte message plus checksum. Its followed by 8-byte RTC data plus checksum.  A checksum counts *all* bytes which come before it, including the bytes of previous data lines with their checksums included too.  After that are 16 lines containing timer data for weekly, daily and astro (civil dusk) timer. The purpose of the very last line I don't fully understand.

###### First the byte data. See below for more details.

```
  0    1     2     3     4     5     6      7    (8)
--Addressfield--    C.M    G.c    CS
0x80, 0xXX, 0xXX, 0x3.8, 0x2.f, 0x8d, 

0 ------RTC data -------------------------
  ss    mm    hh  wdmsk   DD    MM      WD  BF     CS
0x46, 0x23, 0x03, 0x04, 0x09, 0x11, 0x0.2, 0x02, 0x08, 

1-4 ------Timer-Block
------weekly----------  ------weekly---------- 
--------Sun-----------  -------Mon------------ 
upMM  upHH  doMM  doHH  upMM  upHH  doMM  doHH
0x56, 0x06, 0x45, 0x19, 0x12, 0x07, 0x34, 0x21, 0x38, 

------weekly----------  ------weekly---------- 
-------Tue------------  -------Wed------------
upMM  upHH  doMM  doHH  upMM  upHH  doMM  doHH
0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x4c, 


------weekly----------  ------weekly---------- 
-------Thu------------  -------Fri------------
upMM  upHH  doMM  doHH  upMM  upHH  doMM  doHH
0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x74, 

------weekly----------  ------daily----------- 
-------Sat------------  -------all------------
upMM  upHH  doMM  doHH  upMM  upHH  doMM  doHH
0x56, 0x06, 0x45, 0x19, 0xff, 0x0f, 0xff, 0x0f, 0xbe, 

5-16 ------Astroblock 12 lines ----------
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xb4, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xa0, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x78, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x28, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x88, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x48, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xc8, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xc8, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xc8, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xc8, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xc8, 
0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xc8, 

17 ---last line--
 ???  --addressfield-    ??     ??    ??    ??    ??
0x00, 0x80, 0xXX, 0xXX, 0x53, 0x00, 0x10, 0x11, 0x05, 

```

###### Explaining the byte data in detail


```
--- plain 5 byte  message -----
p.0-2  address-field - 3 byte ID of sender or receiver
p.3.h  C  - 4bit counter which usually increases by button pressing
p.3.l  M  - 4bit Member number of group for central unit or kind of sender
p.4.h  G  - 4bit Group number
p.4.l  c  - 4bit action command like up, down, ...
p.5    CS - 8bit checksum (sum of all bytes which come before)

--- 8 byte data line number 0. RTC data
d.0.0    ss - 8bit seconds in BCD format
d.0.1    mm - 8bit minutes in BCD
d.0.2    hh - 8bit hours in BCD
d.0.3    wdmsk - 8bit week day bit mask. each weekday is represented by a bit (bit 0=sunday)
d.0.4    DD - 8bit day of month in BCD
d.0.5    MM - 8bit month of year in BCD
d.0.6    WD - 8bit weekday as a number (0=sunday)
d.0.7    BF - bit flags
d.0.7.0  BF.0 - random timer
d.0.7.2  BF.2 - daylight saving time
d.0.7.7  BF.7 - sun automatic
d.0.8    checksum

--- 8 byte data lines 1-4: weekly and daily timers
d.1.0 - d.1.3 - weekly timer for Sunday:
d.1.0    upMM - 8bit up-time minutes in BCD  (or ff if disabled)
d.1.1    upHH - 8bit up-time hours in BCD    (or 0f if disabled)
d.1.2    doMM - 8bit down-time minutes in BCD (or ff if disabled)
d.1.3    doHH - 8bit down-time hours in BCD (or 0f if disabeld)
d.1.4 - d.1.7 - weekly timer for Monday
d.1.8    CS  - checksum
d.2.0 - d.2.3 - weekly timer for Tuesday
d.2.4 - d.2.7 - weekly timer for Wednesday
d.2.8    CS
d.3.0 - d.3.3 - weekly timer for Thursday
d.3.4 - d.3.7 - weekly timer for Friday
d.3.8    CS
d.4.0 - d.4.3 - weekly timer for Saturday
d.4.4 - d.4.7 - daily timer
d.4.8    CS

--- 8 byte data lines 5-16: astro timer
... the astro timer  table contains 48 pre-calculated civil dusk + user offset times over half a year.
... the receiver will interpolate in between values
d.5.0    8bit down-time minutes in BCD (or ff if disabled)
d.5.1    8bit down-time hours in BCD (or 0f if disabled)
d.5.2    8bit down-time minutes in BCD (or ff if disabled)
d.5.3    8bit down-time hours in BCD (or 0f if disabled)
...
d.5.8  checksum
...
...
d.16.8 checksum

(note: times like 19:30 are BCD encoded to "0x30, 0x1f" and not to "0x30, 0x19".)


--- 8 byte data line 17: no idea
d.17.0     ???
d.17.1-3 - address-field - 3 byte ID containing sender or receiver ID 
d.17.4     ???
d.17.5     ???
d.17.6     ???
d.17.7     ???
d.17.8     ???

```
