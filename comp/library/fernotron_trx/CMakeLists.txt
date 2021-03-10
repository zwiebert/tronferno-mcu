set(srcs   astro.cc fer_api.cc fer_msg_basic.cc fer_msg_rx.cc fer_msg_tx.cc fer_msg_tx_queue.cc fer_radio_rx.cc 
    fer_radio_tx.cc fer_radio_parity.cc fer_rawmsg_buffer.cc fer_rawmsg_build.cc
    fer_trx_api.cc
    )

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES utils_misc utils_debug 
    PRIV_REQUIRES app_config  utils_time utils_misc cc1101_ook 
     )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})