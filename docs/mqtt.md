#### MQTT Topics you can send to:

* tfmcu/+/cmd - Commands like up, down, stop

    * The plus sign represents the ID of the shutter:
```
       tfmcu/23/cmd       # member 3 of group 2
       tfmcu/102030/cmd   # 6 digit hex-ID
       tfmcu/8090a023/cmd # 6 digit hex-ID + group + member
```

* tfmcu/+/pct - percentages 100 for open, 0 for close

    * the plus sign means the same as in tfmcu/+/cmd above

    * currently only numbers 100 or 0 can be sent. All numbers between are rejected.

* tfmcu/cli  - All CLI commands can be send here

    * Don't terminate commands with a semicolon (like in USB-CLI)

    * Don't send multiple commands at once separated by semicolon

    * You can prepend all commands with the word "cli". This gives access to all
    CLI commands. Only the commands send, timer, config can be used without the cli prefix.


#### MQTT Topics you can subscribe to:

* tfmcu/config_out -  MCU-config data in JSON format

* tfmcu/timer_out - Timer/automatic data  in JSON format

* tfmcu/+/pct_out - current shutter position in percent (+ stands for shutter ID)
