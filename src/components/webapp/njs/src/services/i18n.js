
import { getLocaleFromNavigator, init, addMessages, _ } from 'svelte-i18n';

const en = {
    "app": {
        "run":"Run",
        "reload":"Reload",
        "restartMcu":"Restart MCU",
        "save":"Save",
        "hide":"Hide",
        "nav_main_move":"Move",
        "nav_main_percent":"Pct",
        "nav_main_auto":"Auto",
        "nav_main_config":"Settings",
        "nav_main_firmware":"Firmware",
        "nav_cfg_mcu":"MCU",
        "nav_cfg_aliases":"Aliases",
        "nav_cfg_durations":"Durations",
        "nav_cfg_name":"Name",
        "nav_cfg_app":"App",
        "msg_waitForMcuRestart":"Wait for MCU to restart...",
        "msg_firmwareIsUpdating":"Firmware is updating...",
        "msg_enterNewName":"Enter new name:",
        "config_wizards":"Configuration-Wizards",
        "wizard_cuas":"Scan Central-Unit-ID",
        "direction":"Direction",
        "duration":"Duration (secs)",
        "stopClock":"StopClock",
    }
} ;


const de = {
    "app": {
        "duration":"Dauer (sek)",
        "reload":"Neu Laden",
        "restartMcu":"MCU Neustart",
        "save":"Speichern",
        "hide":"Verstecke",
        "msg_waitForMcuRestart":"MCU wird neu gestartet...",
        "direction":"Richtung",
        "stopClock":"Stopp-Uhr",
        "msg_enterNewName":"Neuer Name:",
 
    }
} ;

function setupI18n({ withLocale: _locale } = { withLocale: 'en' }) {
    addMessages('en', en);
    addMessages('de', de);

    init({
        fallbackLocale: 'en',
        initialLocale: getLocaleFromNavigator(),
      });
}


export { _, setupI18n };

