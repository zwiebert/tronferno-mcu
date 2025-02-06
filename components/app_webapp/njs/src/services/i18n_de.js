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
    help: {
      tab: "?",
      ota: {
        tab: "OTA",
      },
      feedback: {
        tab: "Kontakt",
      },
    },
    cfg: {
      mcu: {
        tab: "MCU",
        network: { tab: "Netzwerk" },
        misc: { tab: "Allgemein" },
      },

      receiver: {
        tab: "Empfänger",
        names: {
          tab: "Namen",
        },

        durations: {
          tab: "Laufzeit",
        },

        rot_dir: {
          tab: "Drehrichtung",
        },
        endpos: {
          tab: "Endpunkte",
        },

        set_mode: {
          tab: "SET",
          activate: {
            tab: "Funktion",
          },
          register_gm: {
            tab: "Zentrale",
          },
          register_tx: {
            tab: "Sender",
          },
        },
      },

      transmitter: {
        tab: "Sender",
        repeater: {
          tab: "Repeater",
        },
        names: {
          tab: "Namen",
        },
        register: {
          tab: "Anmelden",
        },
      },
    },
  },

  panes: {
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

    ota: {
      header: "OTA Firmware Update",
      tt: {
        header: `Firmware Information und "Over the Air" Firmware Update vom GitHub zwiebert/tronferno-mcu-bin Repository`,
      },
    },

    help_doc: {
      header: "Online Documentation",
      starter: {
        url: "docs/starter-de.md",
        txt: "Schnellstart Anleitung",
      },
      home_page: {
        url: "https://zwiebert.github.io/tronferno-mcu/index-de.html",
        txt: "Online Documentation, Home-Page",

      },
      starter_faq: {
        url: "docs/starter_faq-de.md",
        txt: "FAQ Liste",
      },

      mqtt: {
        url: "docs/mqtt-de.md",
        txt: "MQTT Klient",
      },
    },
    help_feedback: {
      header: "Fehler melden, Vorschläge, Rückmeldungen",
      tt: {
        header: `Du hast einen Bug entdeckt, oder möchtest ein neues Feature vorschlagen... Der bevorzugte Weg ist das eröffnen
         eines Issues im GitHub-Repository. So können andere Nutzer sehen ob ein Bug bereits gemeldet wurde, oder können
         hilfreiche Kommentare zu einem Vorschlag abgeben. Bitte vorher schauen ob ein Bug in einem der *beiden* Repositories
         bereits gemeldet wurde.
         Falls du dich nicht extra auf GitHub anmelden möchtest, bitte stattdessen per E-Mail.
          `,
      },
      issues: {
        link_src: {
          info: "Vorschläge und Fehler bitte hier einstellen (Englisch,Deutsch):",
        },

        link_bin: {
          info: "Hier nur Fehler melden (Englisch,Deutsch):",
        },
        link_mail: {
          info: "Mail an Autor/Maintainer (English,German):",
        },
      },
    },
    mcu: {
      network: { header: "Netzwerk-Verbindungen und -Dienste", tt: { header: "--" } },
      misc: { header: "Allgemeine Einstellungen", tt: { header: "--" } },
    },

    rx_names: {
      header: "Empfängernamen",
      tt: {
        header: "Bearbeite Gruppen- und Empfängernamen",
      },
    },

    durations: {
      header: "Motor Laufzeiten",
      tt: {
        header:
          "Konfiguration der Rollladen-Laufzeiten von Offen bis Geschlossen und von Offen bis Sonnenposition. Diese Werte sind nötig zum berechnen der aktuellen Position und um eine geforderte Position zu finden.",
      },
    },

    rot_dir: {
      header: "Motor Drehrichtung",
      tt: {
        header: `Motordrehrichtung prüfen. Drehrichtung ändern. Zuerst Drehrichtung mit Pfeiltasten überprüfen.
             Wenn falsch, dann Drehrichtung mit dem Button umkehren und erneut prüfen.`,
      },
    },
    endpos: {
      header: "Endpunkteinstellung",
      tt: {
        header: "Oberen und unteren Endpunkt einstellen für Fernotron-Rohrmotoren",
      },
    },

    activate: {
      header: "SET-Funktion aktivieren",
      tt: {
        header: `SET-Funktion eines Empfängers aktivieren. Sie haben dann 60 Sekunden Zeit, um
            einen Sender am ausgewählten Empfänger an- oder abzumelden.`,
      },
    },
    register_gm: {
      header: "Melde Programmierzentrale an bei Empfänger",
      tt: {
        header: `Um einen Empfänger über eine Gruppen-/Empfängernummer der Zentrale ansprechbar zu machen:
               1) Aktiviere SET-Funktion bei Empfänger. 2) Übertrage Daten (Timer) unter der gewünschten Gruppen-/Empfängernummer`,
      },
    },
    register_tx: {
      header: "Melde Sender an bei Empfänger",
      tt: {
        header: `Zum anmelden/abmelden eines Sender an einen Empfänger: 1) Aktiviere SET-Funktion bei Empfänger.
               2) Sende STOP vom Sender -oder- wähle Sender-ID hier aus und klicke STOP Knopf.`,
      },
    },

    repeater: {
      header: "Funk-Signalverstärker",
      tt: {
        header: `Verstärkt empfangene Funkbefehle durch erneutes Aussenden zwecks Reichweitenerhöhung.
             Eine Liste aller zu verstärkenden Sender-IDs muss angelegt werden. Nur Sender die auf dieser Liste angemeldet sind, werden verstärkt.
             Bei mehr als einem vorhandenen Signalverstärker-Gerät (wie Fernotron 2427), darf jeder Sender nur bei maximal einem Signalverstärker angemeldet sein.`,
      },
    },
    tx_names: {
      header: "Sender-Namen",
      input: "Name",
      tt: {
        header: "Namen für Sender für bessere Übersichtlichkeit. Ein Name könnte z.B. den genauen Stand-Ort des Senders beschreiben.",
      },
    },
    hs_config: {
      home_assistant: {
        header: "Home Assistant Konfiguration",
        tt: {
          header: '1) Konfiguriert HomeAssistant via MQTT oder 2) Erzeugt eine Konfiguraion zum Einfügen in die Konfigurationsdatei des "Home Assistant" Home-Server',
        },
      },
    },
    register: {
      header: "Sender Anmelden",
      tt: {
        header: `Zum Zuordnen von Bewegungen verursacht von Handsendern und Sonnensensoren muss der MCU bekannt sein, an welchen Empfängern diese angemeldet sind.
              Da diese nicht automatisch erfolgen kann, muss der User diese An-/Abmeldungen hier zusätzlich noch einmal durchführen. 
              Runde Knöpfe auf dieser Seite testen oder ändern die realen An-/Abmeldung. 
               `,
      },
    },
  },

  app: {
    duration: "Dauer (sek)",
    reload: "Neu Laden",
    restartMcu: "MCU Neustart",
    save: "Speichern",
    hide: "Verstecke",

    auto: {
      daily: "Täglich",
      weekly: "Wochentage",
      astro: "Dämmerung",
      random: "Zufall",
      sun: "Sonne",
      manual: "Manuell",
      sendRtc: "Uhrzeit senden an:",
      sendData: "Datenübertragung",
      tt: {
        daily: "Standardschaltzeiten AUF/AB. Gleiche Schaltzeiten für alle Wochentage. Die Einstellung --:-- führt zu keinem Schaltbefehl.",
        weekly: "Wochenprogramm. Zwei Schaltzeiten für AUF/AB pro Wochentag. Ohne Häkchen wird der vorige Wochentag kopiert (für z.B. Mo-Fr und Sa-So)",
        astro: `Dämmerungsautomatik mit Astrofunktion. (Die Dämmerungsautomatik benötigt Längen- und Breitengrad in Konfiguration). Schließt Rollladen zur zivilen Dämmerung.
        Eine Verzögerungszeit kann in Minuten angegeben wodurch die Schließzeit entsprechend später wird. Negative Verzögerungszeit verändert die Schließzeit in Richtung früher.`,
        random:
          "Zufallsautomatik. Bewirkt eine zufällige Verzögerung der Schaltzeiten zwischen 0 und 30 Minuten. Die Zufallsautomatik gilt nicht für Dämmerungszeiten.",
        sun: "Sonnenautomatik. Aktivieren der helligkeitsabhängigen Steuerung mit Funksonnensensor 2440.",
        manual:
          "Automatikbetrieb ausschalten. Deaktiviert Schaltzeiten, Sonnenautomtik, Dämmerungsautomatik, etc. (Überschreibt Daten im Empfänger, aber MCU hält eine Kopie zur Wiederherstellung)",
        sendRtc: "Aktuelle Uhrzeit/Aktuelles Datum zu Empfänger oder Gruppen(n) übertragen.",
        sendISO: "Zu Testzwecken: Sendet ISO Zeit an Empfänger anstatt der aktuellen Zeit aus der internen Echtzeituhr.",
        sendData: `Speichert Einstellungen und überträgt die Daten per Funk zu Empfänger oder Gruppe, was 5 Sekunden dauert.
        Der Empfänger quittiert die erfolgreiche Datenübertragung durch kurzes Einschalten des angeschlossenen Motors.`,
      },
    },

    bog: {
      LCD: {
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
      chose_allRegIds: "Andere",
      chose_rxIds: "RX",
      chose_enterId: "ID",
      regTbl_header1: "Sender-ID",
      regTbl_header2: "ist angemeldet bei:",

      tt: {
        chose_allRegIds: "Woanders angemeldete Sender-IDs",
        chose_regIds: "An dieser Empfängernummer angemeldete Sender-IDs",
        chose_rxIds: "Unregistrierte Sender-IDs empfangen über Funk",
        chose_enterId: "Eingabe einer sechs-stelligen Sender-ID",
        test_selectedId: "Bewege kurz Motor(en) bei denen dieser Sender angemeldet ist",
        send_stop: "Sende STOP um den Sender an- oder abzumelden beim Empfänger mit aktivierter SET Funktion",
        set_function: "Aktiviere SET Funktion beim ausgewählten Empfänger",
        enterName: "Namen für Sender vergeben oder ändern",
        text_enterId: "Sender-ID direkt eingeben oder aus Liste wählen",
        register_id: "An-/Abmelden der ausgewählten Sender-ID an die ausgewählte Empfängernummer",
        register_rf: "An-/Abmelden eines Funksenders an die ausgewählte Empfängernummer. Erst Knopf anklicken und dann STOP am Funksender drücken!",
        registeredToTheseRx: "Anzeigen/Ändern der Anmeldungen eines Senders an den/die Empfänger. Spiegelt nur die realen Anmeldungen mittels SET Funktion.",
      },
    },

    dur: {
      h: "Motor-Laufzeiten",
    },

    sun: {
      move_sun_down: "\u263C\u25bc",
      set_sun_pos: "\u263C\u2699",

      tt: {
        move_sun_down: "Bewege Rollladen herunter zur Sonnenposition. Sonnenautomatik muss dafür aktiviert sein.",
        set_sun_pos: "Sonnenpostion einstellen: setze aktuelle Rollladen-Position als neue Sonnenposition",
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

      warning_html: ` 
        <ul>
        <li>Nutze diese Funktion auf eigenes Risiko oder gar nicht</li>
        <li>Das Bewegen des Rohrmotors über den mechanisch möglichen Bereich hinaus kann den Rollladen beschädigen.</li>
        <li>Überprüfe ob wirklich der richtige Rollladen ausgewählt und teste dies mit harmlosen Hoch/Runter/Stop-Kommandos.</li>
        <li>Nur ein EINZIGER Rohrmotor darf angemeldet sein unter der Gruppen-/Empfängernummer. Andernfalls Funkcode des Rohrmotors zur Adressierung benutzen.</li>
        <li>Zur Sicherheit immer einen Handsender bereit haben um ein STOP zu senden, falls die MCU während der Bewegung abstürzt.</li>
        </ul>`,
      tt: {
        header:
          "Einstellen der Rollladen Endpunkte. Ein physischer Taster muss an der MCU angeschlossen sein. Schritte: 1) Authentisieren 2) Adressieren 3) Bewegung testen 4) Aktivieren 5) Endpunkte setzen 6) Deaktivieren 7) Testen 8) Wiederhohlen oder Beenden",
        enter: "Öffne die Steuerung zum Einstellen der Endpunkte.",
        authenticate: "Authentisiere den Benutzer durch drücken des Setzknopfes am MCU Gerät",
        enable: "Aktiviere Funkkommandos für Endpunkte. Zeigt Hoch/Runter Knöpfe zum Einstellen des oberen/unteren Endpunktes.",
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
    downToZero: "\u25bc 100% bis 0%",
    downSunPos: "\u25bc 100% bis Sonnenpos.",
    upTo100: "\u25b2  0% bis 100%",

    rotDir: {
      toggle: "Umkehren",
      //test_up: "Test \u25b2",
      //test_down: "Test \u25bc",
      tt: {
        toggle: "Ändere Drehrichtung. Jeder Klick kehrt die Drehrichtung um.",
        test_up: "Teste Drehrichtung. Panzer sollte ein Stück nach oben fahren.",
        test_down: "Teste Drehrichtung. Panzer sollte ein Stück nach unten fahren.",
      },
    },

    setMode: {
      set_button: "SET-Knopf am Empfängergerät drücken.",
      set_button_radio: "Taster",
      register_unregister: "An- oder Abmelden an Empfänger mit aktiver SET-Funktion",
      register_cu: "Zentrale",
      register_cu_bt: "Melde Zentrale-G-E beim Empfänger an",
      set_mode: "SET",
      dev_address: "Geräte Adresse",
      receiver_address: "Empfänger Adresse",
      sender_address: "Sender",
      gm_address: "G/E/Funkcode",
      h_set_mode_enable: "SET-Funktion eines Empfängers aktivieren",
      h_register_receiver_as_gm: "Melde Zentrale an den Empfänger mit aktiver SET-Funktion an",
      h_register_sender_to_smr: "Melde Sender an Empfänger mit aktiver SET-Funktion an",
      tt: {
        set_button: "Hier drücken um zu sehen wie lange die SET Funktion des Empfängers ativiert ist. Hat sonst keine echte Funktion.",
        set_mode:
          "Aktiviere die SET-Funktion eines Empfängers über Funk. Hinweis: Besonders nützlich für Empfänger mit schwer zugänglicher SET Taste, aber auch sonst.",
        register_cu_bt: `Übertrage Automatic-Daten an Empfänger "E" in Gruppe "G". Bei aktivierter SET Funktion: Melde Empfänger an Programmierzentrale an oder ab.`,
        register_cu:
          "Anmelden der Programmierzentrale (MCU) am Empfänger. Der Empfänger reagiert dann auf Kommandos von dieser Zentrale für die angegebene Gruppen-/Empfängernummer.",
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

    repeater: {
      ids: {
        header: "Senderliste",
        add_button: "Füge hinzu",
        remove_button: "Entferne",
        tt: {
          header:
            "Liste der zu verstärkenden Sender. Alle anderen Sender werden ignoriert. Keine Mehrfachanmeldung bei mehreren Signalverstärkern erlaubt (führt zu Ping-Pong!)",
          add_button: "Add",
          remove_button: "Remove",
        },
      },
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
      { date: "2022", text: "Funktionalität: Drehrichtung. Endpunkte. Funk-Repeater" },
      { date: "2021", text: "Hardware: Ethernet/LAN, CC1101-Funkmodul" },
    ],
  },
};
