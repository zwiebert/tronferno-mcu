export const de = {
  weekdays: ["Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag", "Sonntag"],
  fernotron: {
    register: "anmelden",
    unregister: "abmelden",
    Register: "Anmelden",
    Unregister: "Abmelden",
  },
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
        user_level: {
          name: {
            developer:"Entwickler",
            expert:"Experte",
            admin:"Administrator",
            user:"Benutzer",
            kid:"Kind",
          },
        },
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
      sendRtc: "Uhrzeit senden an:",
    },

    cfg: {
      mcu: {
        user_gpio: {
          add_label: "User-GPIOs",
          add_button: "Hinzufügen",

          tt: {
            gpioxx: "Konfiguriere oder entferne einen User-definierten GPIO hier",
            add_label: "Füge User-definierte GPIOs hinzu welche dann oben konfiguriert werde können. Zugriff dann über MQTT.",
          },
        },
      },
    },
    id: {
      chose_header: "Sender-ID Auswahl/Eingabe",
      chose_allRegIds: "Alle",
      chose_rxIds: "RX",
      chose_enterId: "Eingabe",
      regTbl_header1: "Sender-ID",
      regTbl_header2: "ist angemeldet bei:",

      tt: {
        chose_allRegIds: "Alle angemeldeten Sender-IDs",
        chose_regIds: "An dieser Empfängernummer angemeldete Sender-IDs",
        chose_rxIds: "Alle Sender-IDs empfangen über Funk",
        chose_enterId: "Eingabe einer Sender-ID",
        text_enterId: "Sender-ID direkt eingeben oder aus Liste wählen",
        register_id: "An-/Abmelden der ausgewählten Sender-ID an die ausgewählte Empfängernummer",
        register_rf: "An-/Abmelden eines Funksenders an die ausgewählte Empfängernummer. Erst Knopf anklicken und dann Stop am Funksender drücken!",
      },
    },

    dur: {
      h: "Motor-Laufzeiten",
    },

    sun: {
      move_sun_down: "\u263C\u25bc",
      set_sun_pos: "\u263C\u2699",

      tt: {
        move_sun_down: "Bewege Rolladen herunter zur Sonnenposition. Sonnenautomatik muß dafür aktiviert sein.",
        set_sun_pos: "Speicher aktuelle Rollladen-Position als neue Sonnenposition",
      },
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
    names: {
      restoreNames: "Namen wiederherstellen",
      tt: {
        restoreNames:
          "Wurde die MCU erneuert oder der MCU-Flash-Speicher gelöscht, können hier die Namen aus dem lokalen Browser-Speicher an die MCU gesendet werden.",
      },
    },
  },
  firmware: {
    latest_master: "Letzte Release",
    latest_beta: "Letzte Beta",
    version: "Versions-Nummer",
  },
  help: {
    hint_shutterMisc: "Sonstinge Einstellungen je Rollladen",
    hint_shutterDurations:
      "Konfiguration der Rollladen-Laufzeiten von Offen bis Geschlossen und von Offen bis Sonnenposition. Diese Werte sind nötig zum berechnen der aktuellen Position und um eine geforderte Position zu finden.",
    hint_durStopClock:
      "Zum Messen der Dauer der Rollandenbewegungen. Drücke Start/Stop zum gleichzeitigen Starten von Bewegung und Stoppuhr. Sobald der Rollladen stoppt, Start/Stop erneut drücken um die Stoppuhr anzuhalten.",
    hint_shutterAlias: "Zum Zuordnen von Bewegungen verursacht von Handsendern und Sonnensensoren müssen diese hier erneut angemeldet werden.",
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
      {
        date: "2021-03",
        text:
          "Konfiguration: Alle Defaultwerte für GPIO-Pins wurden entfernt. War Senden=22, Empfangen=17 (tx=15 und rx=16 bei LAN-Version). Bitte manuell konfigurieren!",
      },
      { date: "2021-03", text: "Neue Hardware: Ethernet/LAN, CC1101-Funkmodul" },
    ],
  },
};
