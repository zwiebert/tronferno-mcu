set(srcs 
    gmbitmask_kvs.cc fer_setup.cc
 
    alias/pairings.cc alias/pairings_kvs.cc
    auto/timer_data.cc  auto/timer_data_fs.cc auto/timer_data_kvs.cc auto/timer_minutes.cc auto/timer_event.cc auto/next_timer_event.cc
    cuas/cuid_auto_set.cc
    pos/commands.cc pos/shutter_pct.cc pos/shutter_prefs_kvs.cc pos/move.cc pos/move_start.cc pos/move_stop.cc pos/move_buf.cc
    sep/set_endpos.cc
    sep/auth_button.cc
    repeater/repeater.cc
    txtio/fer_print.cc
    )

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES utils_misc fernotron_trx
    PRIV_REQUIRES main_loop app_config utils_debug key_value_store utils_time gpio storage txtio fernotron_trx fernotron_uout uout app_uout
     )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
