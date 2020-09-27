#interface library for fernotron_trx

idf_component_register(
    INCLUDE_DIRS "./include"
   REQUIRES fernotron_trx app_config dbg
    PRIV_REQUIRES fernotron_trx fernotron_app
     )

