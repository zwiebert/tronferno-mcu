set(srcs astro.cc  fer_api.cc  fer_msg_basic.cc  fer_msg_rx.cc  fer_msg_tx.cc  fer_msg_tx_queue.cc fer_radio_rx.cc fer_radio_tx.cc fer_radio_parity.cc fer_rawmsg_buffer.cc  fer_rawmsg_build.cc  gmbitmask_kvs.cc)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES misc 
    PRIV_REQUIRES app_config app debug key_value_store misc_time config fernotron_pos)

component_compile_options(${component_compile_opts})