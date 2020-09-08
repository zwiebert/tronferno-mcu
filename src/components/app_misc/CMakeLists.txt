set(srcs esp32/timer.cc rtc.cc ) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    REQUIRES app_config misc_time
    PRIV_REQUIRES   config
    EMBED_TXTFILES ./ca_cert.pem
)

component_compile_options(${component_compile_opts})