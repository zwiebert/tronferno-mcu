set(srcs fernotron.cc esp32/timer.cc esp32/main.cc esp32/main_setup.cc esp32/main_loop.cc rtc.cc esp32/unity_setup.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    PRIV_INCLUDE_DIRS "./include/app" "../../esp32" 
    REQUIRES userio main firmware_update
    PRIV_REQUIRES fernotron gpio net misc
    EMBED_TXTFILES ./esp32/ca_cert.pem
)

component_compile_options(${component_compile_opts})