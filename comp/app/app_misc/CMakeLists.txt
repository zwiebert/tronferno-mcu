set(srcs esp32/timer.cc rtc.cc  opt_map_consteval.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES  utils_time firmware_update
    PRIV_REQUIRES   app_settings app_config
    #EMBED_TXTFILES ./ca_cert.pem
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})