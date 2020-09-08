set(srcs fernotron_app.cc esp32/timer.cc esp32/main.cc rtc.cc ) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    PRIV_INCLUDE_DIRS "./include/app" "../../esp32" 
    REQUIRES userio main firmware_update misc_time debug
    PRIV_REQUIRES   misc key_value_store net  storage config_kvs
    EMBED_TXTFILES ./ca_cert.pem
)

component_compile_options(${component_compile_opts})