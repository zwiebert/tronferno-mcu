set(srcs 
    basic/astro.cc basic/fer_api.cc basic/fer_msg_basic.cc basic/fer_msg_rx.cc basic/fer_msg_tx.cc basic/fer_msg_tx_queue.cc basic/fer_radio_rx.cc 
    basic/fer_radio_tx.cc basic/fer_radio_parity.cc basic/fer_rawmsg_buffer.cc basic/fer_rawmsg_build.cc basic/gmbitmask_kvs.cc
 
    alias/pairings.cc alias/pairings_kvs.cc
    auto/timer_data.cc  auto/timer_data_fs.cc auto/timer_data_kvs.cc auto/timer_minutes.cc auto/timer_event.cc
    cuas/cuid_auto_set.cc
    pos/commands.cc pos/shutter_pct.cc pos/shutter_prefs_kvs.cc pos/move.cc pos/move_start.cc pos/move_stop.cc pos/move_buf.cc
    sep/set_endpos.cc
    txtio/fer_print.cc
    )

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES misc 
    PRIV_REQUIRES app_config app debug key_value_store misc_time config userio_app cli cli_app storage
     )

component_compile_options(${component_compile_opts})