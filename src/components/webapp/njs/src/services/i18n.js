
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
        "downToZero":"down to 0%",
        "downSunPos":"down to sun position",
        "upTo100":"up to 100%",
    },
    "help": {
        "hint_durStopClock":"To measure the duration of up/down movements, press Start/Stop to start both movement and clock. After the movement stops by itself, press again to stop the clock.",
        "hint_shutterAlias":"To register position changes made by commands sent from plain controllers and sun-sensors, these controllers have to be registered here. Select G/E above and pair the controller here by start pairing and pressing STOP on the controller.",
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
        "downToZero":"Abwärts bis 0%",
        "downSunPos":"Abwärts bis Sonnenposition",
        "upTo100":"Aufwärts bis 100%", 
    },
    "help": {
        "hint_durStopClock":"Zum Messen der Dauer der Rollandenbewegungen. Drücke Start/Stop zum gleichzeitigen Starten von Bewegung und Stoppuhr. Sobald der Rollladen stoppt, Start/Stop erneut drücken um die Stoppuhr anzuhalten.",
        "hint_shutterAlias":"Zum Zuordnen von Bewegungen durch Kommandos von Handsendern und Sonnensensoren müssen diese registriert werden. Bitte G/E auswählen und Pair oder Unpair starten, dann STOP am Handsender drücken.",
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

