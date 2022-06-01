import { getLocaleFromNavigator, init, addMessages, _ } from "svelte-i18n";

const en = {
  weekdays: ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"],
  app: {
    run: "Run",
    reload: "Reload",
    restartMcu: "Restart MCU",
    save: "Save",
    hide: "Hide",

    navTab: {
      main: {
        move: "\u25b3\u25bd",
        percent: "%%",
        config: "\u2699",
        auto: "Auto",
        firmware: "Firmware",
      },
      cfg: {
        mcu: "MCU",
        aliases: "Aliases",
        sender: "Sender",       
        positions: "Positions",
        name: "Misc",
        set_mode: "Set",
        app: "App",
      },
      sender: {
        aliases: "Aliases",
        repeater: "Repeater",
        transmit: "Transmit",
      },
      positions: {
        aliases: "Aliases",
        durations: "Duration",
      },

    },

    auto: {
      daily: "Daily",
      weekly: "Weekly",
      astro: "Dusk",
      random: "Random",
      sun: "Sun",
      manual: "Manual",
      sendRtc: "Send wall time to:",
    },


    id: {
      chose_header: "Pick/Enter a Sender-ID",
      chose_allRegIds: "All",
      chose_rxIds: "RX",
      chose_enterId: "Enter",
      test_selectedId: "Test",
      tt: {
        chose_allRegIds: "Any registered Sender-IDs",
        chose_regIds: "Sender-IDs registered to this receiver number only",
        chose_rxIds: "List of Sender-IDs recveived via RF",
        chose_enterId: "Enter a Sender-ID",
        text_enterId: "Enter an ID or select ID from a list",
        test_selectedId: "Identify motor(s) paired with this ID by moving them",
      },
    },

    dur: {
      h: "Duration of Shutter-Movements",
    },

    msg_waitForMcuRestart: "Wait for MCU to restart...",
    msg_firmwareIsUpdating: "Firmware is updating...",
    msg_firmwareUpdSuccess: "Update succeeded",
    msg_enterNewName: "Shutter-Name",
    hdr_motorRotDir: "Motor Rotation Direction",
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
      register_cu_bt: "Register central-unit/G/E to Receiver",
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
    hint_shutterMisc:
    "Miscelleanous settings per shutter",
    hint_shutterDurations:
      "Configute here the durations for the shutter to move from open to close and from open to sup-position. This is needed to calculate the current in-between shutter position and to reach a requested in-between position.",
    hint_durStopClock:
      "To measure the duration of up/down movements, press Start/Stop to start both movement and clock. After the movement stops by itself, press again to stop the clock.",
    hint_shutterAlias:
      "To register position changes caused by plain controllers and sun-sensors, these controllers have to be registered here.",
    hint_repeater:
      "Repeat RF commands to increase reach. You have to provide a list of IDs. Only senders from this list will be repeated.",
  },
  mcuConfigNames: {
    verbose: "CLI Verbosity",
    cu: "Central-Unit-ID",
    "astro-correction": "Astro Correction",
    rtc: "Date/Time",
    tz: "Timezone (Posix)",
    "lan-phy": "PHY-Chip",
    "lan-pwr-gpio": "Power-ON/OFF GPIO",
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
    longitude: "Longitude",
    latitude: "Latitude",
    network: "Chose Connection",
    "rf-trx": "RF-modules",
    "rf-tx-pin": "RF-Sender",
    "rf-rx-pin": "RF-Receiver",
    "set-button-pin": "Set-Button",
    "rf-sclk-pin": "CC1101 SCLK",
    "rf-miso-pin": "CC1101 SO",
    "rf-mosi-pin": "CC1101 SI",
    "rf-ss-pin": "CC1101 CS0",
    "gm-used": "Receivers",
    cc1101: {
      "rf-sclk-pin": "CC1101 SCLK",
      "rf-miso-pin": "CC1101 SO",
      "rf-mosi-pin": "CC1101 SI",
      "rf-ss-pin": "CC1101 CS0",
      "rf-tx-pin": "CC1101 GD0 (TX)",
      "rf-rx-pin": "CC1101 GD2 (RX)",
    },
  },

  mcuConfig: {
    ethernet: "LAN/Ethernet",
    wlan_station: "WLAN Station",
    network: "Network-Connection and -Services",
    misc: "Other Settings",
    ntp_client: "NTP Client",
    mqtt_client: "MQTT Client",
    http_server: "Webserver",
    pin_gpio: "Elektrical In-/Outputs",
    config_wizards: "Configurations-Wizards",
    gm_used: "Number of Receivers per Group",
  },

  notifyUser: {
    caption: "Important firmware changes",
    messages: [
      { date: "2021-03", text: "Configuration: Default values for GPIOP-pings removed. Used to be (tx=22, rx=17 (tx=15, rx=16 for LAN version). Please configure manually!" },
      { date: "2021-03", text: "Hardware added: Ethern/LAN. CC1101 RF transceiver module." },
    ],
  },
};

const de = {
  weekdays: ["Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag", "Sonntag"],
  app: {
    duration: "Dauer (sek)",
    reload: "Neu Laden",
    restartMcu: "MCU Neustart",
    save: "Speichern",
    hide: "Verstecke",

    navTab: {
      main: {
        auto: "Auto",
        firmware: "Firmware",
      },
      cfg: {
        mcu: "MCU",
        positions: "Positionen",
        name: "Sonstiges",
        set_mode: "Set",
        app: "App",
      },
      sender: {
        aliases: "Aliases",
        repeater: "Repeater",
        transmit: "Transmit",
      },
      positions: {
        aliases: "Aliases",
        durations: "Laufzeit",
      },
    },

    auto: {
      daily: "Täglich",
      weekly: "Wochentage",
      astro: "Dämmerung",
      random: "Zufall",
      sun: "Sonne",
      manual: "Manuell",
      manual: "Manual",
      sendRtc: "Uhrzeit senden an:",
    },

    id: {
      chose_header: "Sender-ID Auswahl/Eingabe",
      chose_allRegIds: "Alle",
      chose_rxIds: "RX",
      chose_enterId: "Eingabe",
      tt: {
        chose_allRegIds: "Alle angemeldeten Sender-IDs",
        chose_regIds: "An dieser Empfängernummer angemeldete Sender-IDs",
        chose_rxIds: "Alle Sender-IDs empfangen über Funk",
        chose_enterId: "Eingabe einer Sender-ID",
        text_enterId: "Sender-ID direkt eingeben oder aus Liste wählen",
      },
    },

    dur: {
      h: "Motor-Laufzeiten",
    },

    msg_waitForMcuRestart: "MCU wird neu gestartet...",
    msg_firmwareIsUpdating: "Firmware wird aktualisiert...",
    msg_firmwareUpdSuccess: "Aktualisierung erfolgreich",
    direction: "Richtung",
    stopClock: "Stopp-Uhr",
    msg_enterNewName: "Rollladen-Name",
    hdr_motorRotDir: "Motor Drehrichtung",
    downToZero: "Abwärts bis 0%",
    downSunPos: "Abwärts bis Sonnenposition",
    upTo100: "Aufwärts bis 100%",
    setMode: {
      set_button: "Set-Knopf am Empfängergerät drücken und hier klicken.",
      set_button_radio: "Taster",
      register_unregister: "An- oder Abmelden an Empfänger im Eingabe-Modus",
      register_cu: "Zentrale",
      register_cu_bt: "Melde Zentrale/G/E beim Empfänger an",
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
    hint_shutterMisc:
    "Sonstinge Einstellungen je Rollladen",
    hint_shutterDurations:
    "Konfiguration der Rollladen-Laufzeiten von Offen bis Geschlossen und von Offen bis Sonnenposition. Diese Werte sind nötig zum berechnen der aktuellen Position und um eine geforderte Position zu finden.",
    hint_durStopClock:
      "Zum Messen der Dauer der Rollandenbewegungen. Drücke Start/Stop zum gleichzeitigen Starten von Bewegung und Stoppuhr. Sobald der Rollladen stoppt, Start/Stop erneut drücken um die Stoppuhr anzuhalten.",
    hint_shutterAlias:
      "Zum Zuordnen von Bewegungen verursacht von Handsendern und Sonnensensoren müssen diese hier registriert werden.",
  },
  mcuConfigNames: {
    verbose: "CLI Verbose Level",
    cu: "Zentrale ID",
    "astro-correction": "Astro Korrektur",
    rtc: "Datum/Uhrzeit",
    tz: "Zeitzone (Posix)",
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
    longitude: "Längengrad",
    latitude: "Breitengrad",
    network: "Wähle Verbindung",
    "rf-trx": "Funkmodule",
    "rf-tx-pin": "Funk-Sender",
    "rf-rx-pin": "Funk-Empfänger",
    "set-button-pin": "Setz-Knopf",
    "cc1101-enable": "Verwende CC1101",
    "gm-used": "Empfänger",
  },

  mcuConfig: {
    wlan_station: "WLAN Station",
    network: "Netzwerkverbindung und -dienste",
    misc: "Sonstige Einstellungen",
    ntp_client: "NTP Klient",
    mqtt_client: "MQTT Klient",
    http_server: "Webserver",
    pin_gpio: "Elektrische Ein- und Ausgänge",
    config_wizards: "Konfigurations-Assistenten",
    gm_used: "Anzahl Empfänger je Gruppe",
  },

  notifyUser: {
    caption: "Wichtige Firmware-Änderungen",
    messages: [
      { date: "2021-03", text: "Konfiguration: Alle Defaultwerte für GPIO-Pins wurden entfernt. War Senden=22, Empfangen=17 (tx=15 und rx=16 bei LAN-Version). Bitte manuell konfigurieren!" },
      { date: "2021-03", text: "Neue Hardware: Ethernet/LAN, CC1101-Funkmodul" },
    ],
  },

};

function setupI18n() {
  addMessages("en", en);
  addMessages("de", de);

  init({
    fallbackLocale: "en",
    initialLocale: getLocaleFromNavigator(),
  });
}

export { _, setupI18n };
