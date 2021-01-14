import { getLocaleFromNavigator, init, addMessages, _ } from "svelte-i18n";

const en = {
  app: {
    run: "Run",
    reload: "Reload",
    restartMcu: "Restart MCU",
    save: "Save",
    hide: "Hide",


    navTab:{
        main:{
            move: "\u25b3\u25bd",
            percent: "%%",
            config: "\u2699",
            auto: "Auto",
            firmware: "Firmware",
        },
        cfg:{
            mcu: "MCU",
            aliases: "Aliases",
            durations: "Duration",
            name: "Name",
            set_mode: "Set",
            app: "App",
        },
    },

    auto:{
        daily:"Daily",
        weekly:"Weekly",
        astro:"Dusk",
        random:"Random",
        sun:"Sun",
        manual:"Manual",
      },

    dur:{
        h:"Duration of Shutter-Movements",
    },

    msg_waitForMcuRestart: "Wait for MCU to restart...",
    msg_firmwareIsUpdating: "Firmware is updating...",
    msg_enterNewName: "Enter new name:",
    config_wizards: "Configuration-Wizards",
    wizard_cuas: "Scan Central-Unit-ID",
    direction: "Direction",
    duration: "Duration (secs)",
    stopClock: "StopClock",
    downToZero: "down to 0%",
    downSunPos: "down to sun position",
    upTo100: "up to 100%",
    setMode: {
      set_button: "Press Set-Button on Receiver first. Then click here.",
      set_button_radio: "Button",
      register_unregister: "Register/Unregister",
      register_cu_bt:"Register central-unit/G/E to Receiver",
      register_cu: "Central-Unit",
      register_sender: "Sender",
      set_mode: "Enable Set-Mode",
      dev_address: "Device Address",
      receiver_address: "Receiver Address",
      sender_address: "Sender",
      gm_address: "Central",
      motor_code: "Motor-Code",
      h_set_mode_enable: "Enable Set Mode",
      h_register_receiver_as_gm: "Register Central-Unit to Receiver in Set-Mode",
      h_register_sender_to_smr: "Register a Sender to Receiver in Set-Mode",
    },
  },
  firmware: {
    latest_master: "Latest Release",
    latest_beta: "Latest Beta",
    version: "Version-Number",
    url: "URL",
  },
  help: {
    hint_durStopClock:
      "To measure the duration of up/down movements, press Start/Stop to start both movement and clock. After the movement stops by itself, press again to stop the clock.",
    hint_shutterAlias:
      "To register position changes made by commands sent from plain controllers and sun-sensors, these controllers have to be registered here. Select G/E above and pair the controller here by start pairing and pressing STOP on the controller.",
  },
  mcuConfigNames: {
    "verbose": "CLI Verbosity",
    "cu": "Central-Unit-ID",
    "astro-correction": "Astro Correction",
    "rtc": "Date/Time",
    "tz": "Timezone (Posix)",
    "wlan-ssid": "SSID",
    "wlan-password": "Password",
    "ntp-server": "NTP-Server",
    "mqtt-enable": "On/Off",
    "mqtt-url": "Server-URL",
    "mqtt-user": "Login-Name",
    "mqtt-password": "Login-Password",
    "mqtt-client-id": "Client-ID",
    "mqtt-root-topic": "Root-Topic",
    "http-enable": "On/Off",
    "http-user": "Auth-Name",
    "http-password": "Auth-Password",
    "longitude": "Longitude",
    "latitude": "Latitude",
    "network": "Chose Connection",
    "rf-tx-pin": "RF-Sender",
    "rf-rx-pin": "RF-Receiver",
    "set-button-pin": "Set-Button",
    "gm-used": "Receivers",
  },

  mcuConfig: {
    "wlan_station": "WLAN Station",
    "network": "Network-Connection and -Services",
    "misc": "Other Settings",
    "ntp_client": "NTP Client",
    "mqtt_client": "MQTT Client",
    "http_server": "Webserver",
    "pin_gpio": "Elektrical In-/Outputs",
    "config_wizards": "Configurations-Wizards",
    "gm_used": "Number of Receivers per Group",
  },
};

const de = {
  app: {
    duration: "Dauer (sek)",
    reload: "Neu Laden",
    restartMcu: "MCU Neustart",
    save: "Speichern",
    hide: "Verstecke",

    navTab:{
        main:{
            auto: "Auto",
            firmware: "Firmware",
        },
        cfg:{
            mcu: "MCU",
            aliases: "Alias",
            durations: "Laufzeit",
            name: "Name",
            set_mode: "Set",
            app: "App",
        },
    },

    auto:{
        daily:"Täglich",
        weekly:"Wochentage",
        astro:"Dämmerung",
        random:"Zufall",
        sun:"Sonne",
        manual:"Manuell",
      },

      dur:{
        h:"Motor-Laufzeiten",
    },


    msg_waitForMcuRestart: "MCU wird neu gestartet...",
    direction: "Richtung",
    stopClock: "Stopp-Uhr",
    msg_enterNewName: "Neuer Name:",
    downToZero: "Abwärts bis 0%",
    downSunPos: "Abwärts bis Sonnenposition",
    upTo100: "Aufwärts bis 100%",
    setMode: {
      set_button: "Set-Knopf am Empfängergerät drücken und hier klicken.",
      set_button_radio: "Taster",
      register_unregister: "An- oder Abmelden an Empfänger im Eingabe-Modus",
      register_cu: "Zentrale",
      register_cu_bt:"Melde Zentrale/G/E beim Empfänger an",
      set_mode: "Aktiviere Set-Modus",
      dev_address: "Geräte Adresse",
      receiver_address: "Empfänger Adresse",
      sender_address: "Sender",
      gm_address: "Zentrale",
      h_set_mode_enable: "Ein Empfängerät in Set/Eingabe-Modus versetzen",
      h_register_receiver_as_gm: "Melde Zentrale an Empfänger-im-Set-Modus an",
      h_register_sender_to_smr: "Melde Sender an Empfänger-im-Set-Modus an",
    },
  },
  firmware: {
    latest_master: "Letzte Release",
    latest_beta: "Letzte Beta",
    version: "Versions-Nummer",
  },
  help: {
    hint_durStopClock:
      "Zum Messen der Dauer der Rollandenbewegungen. Drücke Start/Stop zum gleichzeitigen Starten von Bewegung und Stoppuhr. Sobald der Rollladen stoppt, Start/Stop erneut drücken um die Stoppuhr anzuhalten.",
    hint_shutterAlias:
      "Zum Zuordnen von Bewegungen durch Kommandos von Handsendern und Sonnensensoren müssen diese registriert werden. Bitte G/E auswählen und Pair oder Unpair starten, dann STOP am Handsender drücken.",
  },
  mcuConfigNames: {
    "verbose": "CLI Verbose Level",
    "cu": "Zentrale ID",
    "astro-correction": "Astro Korrektur",
    "rtc": "Datum/Uhrzeit",
    "tz": "Zeitzone (Posix)",
    "wlan-ssid": "SSID",
    "wlan-password": "Passwort",
    "ntp-server": "Server-URL",
    "mqtt-enable": "Ein/Aus",
    "mqtt-url": "Server-URL",
    "mqtt-user": "Login-Name",
    "mqtt-password": "Login-Passwort",
    "mqtt-client-id": "Klient-ID",
    "mqtt-root-topic": "Root-Topic",
    "http-enable": "Ein/Aus",
    "http-user": "Auth-Name",
    "http-password": "Auth-Passwort",
    "longitude": "Längengrad",
    "latitude": "Breitengrad",
    "network": "Wähle Verbindung",
    "rf-tx-pin": "Funk-Sender",
    "rf-rx-pin": "Funk-Empfänger",
    "set-button-pin": "Setz-Knopf",
    "gm-used": "Empfänger",
  },

  mcuConfig: {
    "wlan_station": "WLAN Station",
    "network": "Netzwerkverbindung und -dienste",
    "misc": "Sonstige Einstellungen",
    "ntp_client": "NTP Klient",
    "mqtt_client": "MQTT Klient",
    "http_server": "Webserver",
    "pin_gpio": "Elektrische Ein- und Ausgänge",
    "config_wizards": "Konfigurations-Assistenten",
    "gm_used": "Anzahl Empfänger je Gruppe",

  }
};

function setupI18n({ withLocale: _locale } = { withLocale: "en" }) {
  addMessages("en", en);
  addMessages("de", de);

  init({
    fallbackLocale: "en",
    initialLocale: getLocaleFromNavigator(),
  });
}

export { _, setupI18n };
