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

One device broadcasts a message on 443 MHz and all receiving devices listen and filter out the messages addressed to them, or the group they belong to. They cannot acknowledge to the receiver, that they received the data.  They can only give feedback to the user itself, by moving the shutter a bit when they have succesfully received a command which not involves moving the shutter directly.


Each device has an unique ID. On motors this ID is usually printed on the motor itself and on some sticker wich can be placed near the shutter. That ID can be used to access the motor without having to press the pyhsical set button, which may not easy accessible.

Controller devices like 2411 or 2431 have an ID too, but its not printed on anywhere. They can be only seen by looking at the messages they send.


Each message has an address field.  This address can contain a Sender ID (like From:) or an Receiver ID (like To:).

A receiver will listen to all addresses which are equal to its own receiver ID (because the IDs are unique, all other receivers will ignore these messages).

Each receiver keeps a list in its persistent memory, containing sender IDs he will listen to.  If you ativeate the set function or press the set-button, and send a "stop" message, the ID in that message address field will be added (or removed) from that list.  In case of sent by the central unit (2411), the receiver will also memorize the group and member number (which are also part of the message).


Message Format:  Envelope
=========================

Messages like up/down/stop are are encoded into 5 bytes.  If you counted 6 bytes in log output ... yes its really 6, but the 6th contains just a check-sum of that 5 bytes. You may have looked at the data, which actually is sent via 443 MHz, and it looks much more than just 6 bytes ... ok, there really are 12 words of 10bit. The lower 8bit of each word pair is identical to one of the 6 bytes  (word 0/1 = byte 0, word 2/3 = byte 1, ...  word 10/11 = byte 6). The 2 extra bit in each word contain bit parity and indicate if a word is odd or even-numbered.  ... so its really only 5 data bytes. These contain all the addressing info:
```
 bytes 0,1,2: containing the 3 byte device ID
 byte 3 high nibble: A counter which may increment with each button press. How much it increments depents is not the same for all buttons. message may be discarded if counter stays the same.
 byte 3 low nibble: member number (or type of sender if not sent by central unit)
 byte 4 high nibble: group nunber
 (byte 4 low  nibble: used for message content)
```

Explaining Group and Member nunber
==================================
The central unit group/member numbers to control shutters independly. The central unit 2411 lets you configure 7 groups each having up to 7 members.  You can assign multiple motors to a single Group/Member combo, but not one motor to multiple Group/Member combinations.  So group=1 member=1 could address all windows in a room if you want. But group=1 member=1 and group=2 member=1 cannot address the same motor.


Explaining the Device ID
========================

* 3-byte Device ID, There are different kind of IDs. The kind is encoded in the first nibble (4 bit)  of the ID:  plain-sender: 0x1, sun-sensor: 0x2, central-unit=0x8, receiver=0x9.

   * ID of a receiver (9xxxx):  A motor has a designated 5 digit hex code number. This number is printed on the motor itself and on a few stickers coming with it. 5 digts are 5 nibbles or 2.5 bytes, not 3.  Prefix these 2.5 bytes with number 9. This will be the Device ID of the Receiver.  If, for example, your orignal 2411 central unit becomes destroyed, you need to press the set button of a motor to add it to your new 2411. Or you can add that code into your new 2411 and send the "press the button" command to that motor.

   * ID of central unit (8xxxx). Its not printed out anywhere, because there is no need to know it, when using the original hardware.

  Example: To pair a motor with a central unit, you first press the set button on the motor. Now you send a "Stop" command to Number 3 of Group 2.  To 5-byte command sent will contain:

    * ID of the central Unit, which will the motor stores in its local persistent memory.
    * The group number (1...7) and a group-member number (1...7) which will also be stored. 

   Each motor can store only one central unit ID, groups number and member number. The motor will now listen individually to commands containing that ID and Group/Member number combination. To make more than one motor follow this combo (e.g. large windows with 2 motors), these motors are paired with the same group/member number. . 

 
   To make multiple motors with different group or member number  listen to a single command, the number 0 acts as a wildcard for group and/or member.
   Group=1 and member= 0 targets all members of group 1. Other possible combinations:  group=0 member=0 (all)  group=0 member=1 (not so useful, but it works).


  * ID of sun-sensor (9xxxx):  Each motor can store one sun-sensor ID in its memory. All motors which stored that ID will listen to the commands containing that ID. So one sun-sensor can control multiple motors, but one motor can not contolled by multiple sun-sensors.


  * ID of a plain sender (1xxxx).  Each motor can store *multiple* plain senders and will listen to commands containing one of these IDs.  Like with sun-sensor, multiple motors can be controlled by one plain sender, because you can add its ID to as many motors you want.  But unlike sun-sensor, you can also have multiple plain senders each controlling the same motor. This makes them very versatile:  One plain sender could glued to the window frame and controls just the shutter on this  window. Another one could be glued near the door frame, and control the shutters on all the windows in a room.  Another one could control the entire floor, or even  the entire house.



Message Content
===============

Now for the content. Simple messages can fit into the nibble we have left at the end of the 5 bytes mentioned  above:

byte 4 low nibble: command code number. e.g. stop (encoded as number 3), up (4), down (5), ...

Longer messages are required to set the RTC and set the built-in timers.
```
message stop                           ; 4.5 + 0.5 bytes
message timer rtc-data                 ; 4.5 + 0.5 + 8 bytes
message timer rtc-data  timer-data add ; 4.5 + 0.5 + 8 + (8*16) + 8 bytes 
```


Commands with additinal Data
============================
There are commands that need to come with some data. A command like "set the builtin RTC" would need to submit the current time. Obvsome data to be useful are used to configure the builtin timer of a motor.  The configuratin data follows the command directly. It can be thought of a command having pa  These data usually contains sets of 8 bytes, followed by a checksum and doubled to make a line of 19 10-bit-words.  To set the RTC


How its transmitted
===================
The checksum is added to the 5 byte command and  6th byte is added containing a checksum of the 5 byte command.  These 6 bytes are made into 12 bytes by doubling each byte. So byte 0 becomes byte 0 and 1 ... and byte 5 becomes byte 10 and byte 11.  Then 8bit bytes are extended to 10 bit words.  Bits 0-7 still contain the data byte. Bit 8 is 0 for even words (0, 2, 4 .. 10) and 1 for odd words (1, 3, 5 .. 11).  Bit 9 is for bit parity (so each word has even nummber of 1 and 0 bits).

The 12 words will be send with a pause between each word. This pause its called stop bit in my code. Its like a normal bit with the negative wave extend for the duration of 2 normal bits. Theres also the preamble which starts every transmission.

When setting up the builint RTC and timers in the motor, the data words containing time and timer data are just appended.

So the number of sent words goes from 12 (plain commmand) to 30 (+ RTC data) up to 336 (+ Timer data).  It takes 5 seconds to transmit 336 words.

