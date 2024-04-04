#!/bin/bash --debug

SRCDIR="$1"
BINDIR="$2"
CMD="$3"


scripts="-f $HOME/esp/ft2232.cfg -f /usr/local/share/openocd/scripts/board/esp-wroom-32.cfg"
scripts="-f $HOME/esp/ft2232.cfg -f target/esp32.cfg"
ocd_cmd="openocd $scripts"

part_ota_0=$(grep ota_0 $SRCDIR/partitions.csv | sed -E 's/(.+),(.+),(.+),(.+),(.+),/\4/')
part_ota_0_size=$(grep ota_0 $SRCDIR/partitions.csv | sed -E 's/(.+),(.+),(.+),(.+),(.+),/\5/')
part_otadata=$(grep otadata $SRCDIR/partitions.csv | sed -E 's/(.+),(.+),(.+),(.+),(.+),/\4/')


flash_all() {
$ocd_cmd \
	-c "program_esp  $BINDIR/bootloader/bootloader.bin 0x1000 verify" \
        -c "program_esp  $BINDIR/bootloader/bootloader.bin 0x1000 verify" \
        -c "program_esp  $BINDIR/partition_table/partition-table.bin 0x8000 verify" \
        -c "program_esp  $BINDIR/ota_data_initial.bin $part_otadata  verify" \
        -c "program_esp  $BINDIR/netmcu.bin   $part_ota_0 verify reset exit"
}


flash_app() {
$ocd_cmd \
        -c "program_esp  $BINDIR/ota_data_initial.bin $part_otadata" \
        -c "program_esp  $BINDIR/netmcu.bin  $part_ota_0 reset exit"
}

run_server() {
    echo "Run Server"
    $ocd_cmd
}

kill_server() {
    echo "kill server"
    killall -s SIGINT openocd
}

ocd_wait() {
    echo "Wait..."
    tail --pid=$(pgrep openocd) -f /dev/null
    echo "..done"

}


if [ "xxx$CMD" = "xxxflash_app" ]; then
    kill_server
    sleep 1
    flash_app
elif  [ "xxx$CMD" = "xxxflash" ]; then
    kill_server
    sleep 1
    flash_all
elif  [ "xxx$CMD" = "xxxserver_loop" ]; then
    kill_server
    while true; do
        run_server
        sleep 2;
        ocd_wait
    done
elif  [ "xxx$CMD" = "xxxserver" ]; then
      run_server
else
    echo "Usage: sh esp32_ocd.sh server | server_loop | flash | flash_app"
    echo ""
    echo "server        runs the debug server"
    echo "server_loop   ...like server, but restarts automatically after flashing"
    echo "flash         stop server and writes firmware"
    echo "flash_app     ...like flash, but writes the app.bin only"
fi
