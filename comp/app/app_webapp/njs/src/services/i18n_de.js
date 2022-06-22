export const de = {
  weekdays: ["Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag", "Sonntag"],
  weekdays_short: ["Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"],
  fernotron: {
    register: "anmelden",
    unregister: "abmelden",
    Register: "Anmelden",
    Unregister: "Abmelden",
    RadioCode: "Funkcode",
    radio_code: "Funkcode",
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
     //   firmware: "FW-Update",
        user_level: {
          name: {
            developer: "Entwickler",
            expert: "Experte",
            admin: "Administrator",
            user: "Benutzer",
            kid: "Kind",
          },
        },
      },
      cfg: {
        mcu: "MCU",
        positions: "Positionen",
        name: "Name",
        sender: "Sender",
        receiver: "Empfänger",
        rot_dir: "Drehrichtung",
        endpos: "Endpunkte",
        set_mode: "SET",
        app: "App",
      },
      sender: {
        aliases: "Aliases",
       // repeater: "Repeater",
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

    sep: {
      header: "Endpunkte einstellen",
      enter: "Öffne Endpunkte Einsteller",
      authenticate: "Authentisiere",
      auth_prog_wait: "Vorbereitung... Bitte warten...",
      auth_prog_press: "Drücke Taster am MCU Gerät!",
      enable: "Aktiviere",
      disable: "Deaktiviere",
      exit: "Beenden",

      warning_html: `Zum Einstellen der Rollladen-Endpunkte: 
        <ul>
        <li>Nutze diese Funktion auf eigenes Risiko oder gar nicht</li>
        <li>Das Bewegen des Rohrmotors über den mechanisch möglichen Bereich hinaus kann den Rollladen beschädigen.</li>
        <li>Überprüfe ob wirklich der richtige Rollladen ausgewählt und teste dies mit harmlosen Hoch/Runter/Stop-Kommandos.</li>
        <li>Nur ein EINZIGER Rohrmotor darf angemeldet sein unter der Gruppen-/Empfängernummer. Anderfalls Funkcode des Rohrmotors zur Adressierung benutzen.</li>
        <li>Zur Sicherheit immer einen Handsender bereit haben um ein STOP zu senden, falls die MCU während der Bewegung abstürzt.</li>
        </ul>`,
      tt: {
        header:
          "Einstellen der Rollladen Endpunkte. Ein physischer Taster muss an der MCU angeschlossen sein. Schritte: 1) Authentisieren 2) Adressieren 3) Bewegung testen 4) Aktivieren 5) Endpunkte setzen 6) Deaktvieren 7) Testen 8) Wiederhohlen oder Beenden",
        enter: "Öffne die Steuerung zum Einstellen der Endpunkte.",
        authenticate: "Authentisiere den Benutzer durch drücken des Setzknopfes am MCU Gerät",
        enable: "Aktiviere Funkkommandos für Endpunkte. Zeigt Hoch/Runter Knopfe zum Einstellen des oberen/unteren Endpunktes.",
        disable: "Deaktiviere Funkkommandos für Endpunkte.",
        exit: "Beende Authentisierung und verlasse die Steuerung",
      },
    },

    msg_waitForMcuRestart: "MCU wird neu gestartet...",
    msg_firmwareIsUpdating: "Firmware wird aktualisiert...",
    msg_firmwareUpdSuccess: "Aktualisierung erfolgreich",
    direction: "Richtung",
    stopClock: "Stopp-Uhr",
    msg_enterNewName: "Rollladen-Name",
    hdr_motorRotDir: "Motor Drehrichtung",
    downToZero: "\u25bc 100% bis 0%",
    downSunPos: "\u25bc 100% bis Sonnenpos.",
    upTo100: "\u25b2  0% bis 100%",

    rotDir: {
      toggle: "Umkehren",
      test_up: "Test \u25b2",
      test_down: "Test \u25bc",
      tt: {
        toggle: "Ändere Drehrichtung. Jeder Klick kehrt die Drehrichtung um.",
        test_up: "Teste Drehrichtung. Panzer sollte ein Stück nach oben fahren.",
        test_down: "Teste Drehrichtung. Panzer sollte ein Stück nach unten fahren.",
      },
    },

    setMode: {
      set_button: "SET-Knopf am Empfängergerät drücken und hier klicken.",
      set_button_radio: "Taster",
      register_unregister: "An- oder Abmelden an Empfänger mit aktiver SET-Funktion",
      register_cu: "Zentrale",
      register_cu_bt: "Melde Zentrale-G-E beim Empfänger an",
      set_mode: "Aktiviere SET-Funktion",
      dev_address: "Geräte Adresse",
      receiver_address: "Empfänger Adresse",
      sender_address: "Sender",
      gm_address: "G/E/Funkcode",
      h_set_mode_enable: "SET-Funktion eines Empfängers aktivieren",
      h_register_receiver_as_gm: "Melde Zentrale an den Empfänger mit aktiver SET-Funktion an",
      h_register_sender_to_smr: "Melde Sender an Empfänger mit aktiver SET-Funktion an",
      tt: {
        set_mode: "Aktiviere die SET-Funktion eines Empfängers über Funk. Hinweis: Besonders nützlich für Empfänger mit schwer zugänglicher SET Taste, aber auch sonst.",
        register_cu_bt: "Restriere Programmierzentrale und Gruppe/Empfänger beim Empfänger und übertrage alle Timer-Daten an den Empfänger",
        register_cu: "Anmelden der Programmierzentrale (MCU) am Empfänger. Der Empfänger reagiert dann auf Kommandos von dieser Zentrale für die angegebene Gruppen-/Empfängernummer.",
        register_sender: "Anmelden eines Senders an den Empfänger. Der Empfänger reagiert auf Kommandos von allen registrierten Sendern.",
  
      },
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
    network: "Netzwerk",
    misc: "Allgemein",
    ntp_client: "NTP Klient",
    mqtt_client: "MQTT Klient",
    http_server: "Webserver",
    pin_gpio: "Elektrische Ein- und Ausgänge",
    config_wizards: "Konfigurations-Assistenten",
    gm_used: "Anzahl Empfänger je Gruppe",
  },

  notifyUser: {
    caption: "Firmware-Änderungen",
    messages: [
      { date: "2022", text: "Funktionalität: Drehrichtung umkehren. Endpostionen einstellen. Funk-Repeater" },
      { date: "2021", text: "Hardware: Ethernet/LAN, CC1101-Funkmodul" },
    ],
  },
};