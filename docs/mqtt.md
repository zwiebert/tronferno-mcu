#### MQTT Topics you can send to:

* tfmcu/+/cmd - Commands like up, down, stop
     * The plus sign represents the ID of the shutter:

```
       tfmcu/23/cmd       # member 3 of group 2
       tfmcu/102030/cmd   # 6 digit hex-ID
       tfmcu/8090a023/cmd # 6 digit hex-ID + group + member


     Examples:

     FHEM Set-List Entries:
        stop tfmcu/25/cmd $EVENT      # topic: 'tfmcu/25/cmd' data: 'stop'. Stops shutter 5 of group 2.
        down tfmcu/101234/cmd $EVENT  # topic: 'tfmcu/101234/cmd' data: 'down'. Move down shutters paired with controller 101234.
        up tfmcu/25/cmd $EVENT        # topic: 'tfmcu/10/cmd' data: 'up'.  Move up all shutters of group 1.
        up tfmcu/25/cmd $EVENT        # topic: 'tfmcu/00/cmd' data: 'up'.  Move up all shutters of all groups.
```


* tfmcu/+/pct - percentages 100 for open, 0 for close

     * the plus sign means the same as in tfmcu/+/cmd above

     * percentages other than 100 and 0 only work for group/members of the configured central unit.


```
    Example:
      topic: tfmcu/15/pct data: 33   # set member 5 of group 1 to 33%
```



* tfmcu/cli  - Any [CLI](CLI.md) command can be send here

     * commands can be sent in CLI syntax or in JSON format

     * reply will be sent to tfmcu/cli_out in JSON format

     * if using CLI syntax, don't terminate commands with semicolon or try to send mutiple commands separated by semicolon

     * For better use from FHEM Setlist: you may prepend commands with the word 'cli'. It will be ignored.


```
    Example:
      topic: tfmcu/cli
         data: config all=?
         data: {"config":{"all":"?"}}
         data: cli mcu version=full
         data: {"mcu":{"version":"full"}}


     FHEM setlist:
       cli tfmcu/cli $EVENT          # data: 'config all=?'. This would send $EVENT 'cli config all=?', but the 'cli ' will be ignored.
       config tfmcu/cli $EVENT       # data: 'all=?'. This would send $EVENT 'config all=?'


```


 * tfmcu/gpo/+/level - send 0 or 1 to change level of output  pin

```
    Example:
      topic: tfmcu/gpo/22/level data: 1
```


#### MQTT Topics you can subscribe to:

* tfmcu/cli_out - Response (in JSON) of any CLI command sent to tfmcu/cli.

* tfmcu/+/pct_out - current shutter position in percent (+ stands for shutter ID)

* tfmc/gpi/+/level - change of level (H,L) on input pin (must have been set as input pin in config)
