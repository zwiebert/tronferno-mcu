#!/bin/sh

ocd_cmd="openocd -f $HOME/esp/ft2232.cfg -f /usr/local/share/openocd/scripts/board/esp-wroom-32.cfg"


flash_all() {
openocd -f ~/esp/ft2232.cfg -f /usr/local/share/openocd/scripts/board/esp-wroom-32.cfg \
        -c "program_esp  bootloader/bootloader.bin 0x1000 verify" \
        -c "program_esp  partition_table/partition-table.bin 0x8000 verify" \
        -c "program_esp  tronferno-mcu.bin  0x10000 verify" \
        -c "program_esp  ota_data_initial.bin 0x300000  verify reset exit"
}


flash_app() {
openocd -f ~/esp/ft2232.cfg -f /usr/local/share/openocd/scripts/board/esp-wroom-32.cfg \
        -c "program_esp  tronferno-mcu.bin  0x10000 reset exit"

}


if [ "xxx$1" = "xxxflash_app" ]; then
    killall -s SIGINT openocd
    sleep 1
    flash_app
elif  [ "xxx$1" = "xxxflash" ]; then
    killall -s SIGINT openocd
    sleep 1
    flash_all
elif  [ "xxx$1" = "xxxloop" ]; then
    echo "start loop"
    while true; do $ocd_cmd || sleep 10; done
else
   # $ocd_cmd
fi

#$ocd_cmd
