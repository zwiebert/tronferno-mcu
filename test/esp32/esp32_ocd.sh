#!/bin/sh

ocd_cmd="openocd -f $HOME/esp/ft2232.cfg -f /usr/local/share/openocd/scripts/board/esp-wroom-32.cfg"

flash_all() {
openocd -f ~/esp/ft2232.cfg -f /usr/local/share/openocd/scripts/board/esp-wroom-32.cfg \
        -c "program_esp  bootloader/bootloader.bin 0x1000 verify" \
        -c "program_esp  partition_table/partition-table.bin 0x8000 verify" \
        -c "program_esp  unit-test-app.bin  0x10000 verify" \
        -c "program_esp  ota_data_initial.bin 0x300000  verify reset exit"
}


flash_app() {
openocd -f ~/esp/ft2232.cfg -f /usr/local/share/openocd/scripts/board/esp-wroom-32.cfg \
        -c "program_esp   unit-test-app.bin  0x10000" \
        -c "program_esp  ota_data_initial.bin 0x300000  reset exit"


}

run_server() {
    $ocd_cmd
}

kill_server() {
    killall -s SIGINT openocd
}

ocd_wait() {
        tail --pid=$(pgrep openocd) -f /dev/null

}



if [ "xxx$1" = "xxxflash_app" ]; then
    kill_server
    sleep 1
    flash_app
elif  [ "xxx$1" = "xxxflash" ]; then
    kill_server
    sleep 1
    flash_all
elif  [ "xxx$1" = "xxxserver_loop" ]; then
    kill_server
    while true; do
        run_server
        sleep 2;
        ocd_wait
    done
elif  [ "xxx$1" = "xxxserver" ]; then
      run_server
else
    echo "Usage: sh esp32_ocd.sh server | server_loop | flash | flash_app"
    echo ""
    echo "server        runs the debug server"
    echo "server_loop   ...like server, but restarts automatically after flashing"
    echo "flash         stop server and writes firmware"
    echo "flash_app     ...like flash, but writes the app.bin only"
fi

#$ocd_cmd
