#interface library for fernotron

idf_component_register(
    INCLUDE_DIRS "./include"
   REQUIRES fernotron app_config dbg
    PRIV_REQUIRES fernotron fernotron_app
     )

