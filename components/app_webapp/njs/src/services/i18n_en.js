export const en = {
  weekdays: ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"],
  weekdays_short: ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"],
  fernotron: {
    register: "register",
    unregister: "unregister",
    Register: "Register",
    Unregister: "Unregister",
    RadioCode: "RadioCode",
    radio_code: "radio-code",
  },

  navTab: {
    main: {
      move: "\u25b3\u25bd",
      percent: "%%",
      config: "\u2699",
      auto: "Auto",
      firmware: "OTA",
      user_level: {
        developer: "\uD83D\uDDA5\uD83D\uDDA5",
        all: "$#",
        expert: "\uD83D\uDCBB",
        admin: "\uD83D\uDEE1",
        user: "\u2602",
        kid: "\uD83D\uDEB8",
        name: {
          developer: "Developer",
          expert: "Expert",
          admin: "Admin",
          user: "User",
          kid: "Kid",
        },
      },

      guiacc: {
        tab: "",
      },
    },

    help: {
      tab: "\u2753",
      ota: {
        tab: "OTA",
      },
      hsc: {
        tab: "\uD83C\uDFE0",
      },
      backup: {
        tab: "Backup",
      },
      doc: {
        tab: "\uD83D\uDCD6",
      },
      feedback: {
        tab: "Contact",
      },
    },

    cfg: {
      mcu: {
        tab: "MCU",
        network: { tab: "Network" },
        misc: { tab: "Misc" },
        cc1101: {
          tab: "CC1101",
        },
      },

      receiver: {
        tab: "Receiver",
        names: {
          tab: "Names",
        },
        durations: {
          tab: "Duration",
        },
        rot_dir: {
          tab: "Rotation-Direction",
        },

        endpos: {
          tab: "End-Positions",
        },

        set_mode: {
          tab: "SET",
          activate: {
            tab: "Function",
          },
          register_gm: {
            tab: "Centre",
          },
          register_tx: {
            tab: "Transmitter",
          },
        },
      },

      transmitter: {
        tab: "Transmitter",
        repeater: {
          tab: "Repeater",
        },
        register: {
          tab: "Register",
        },
        names: {
          tab: "Names",
          input: "TX-Name",
        },
      },
    },
  },

  panes: {
    ota: {
      header: "OTA Firmware Update",
      tt: {
        header: "Firmware information and Over the Air firmware update from GitHub zwiebert/tronferno-mcu-bin repository",
      },
    },
    rfcap: {
      header: "RF Capture",
      do_capture: "Start Capture",
      tt: {
        header: "Capture raw radio data and display as hex-dump. ",
        do_capture: "Click here to capture received RF raw data. 1) Hold Stop Button on a Fernotron Transmitter 2) Click this button 3) Data will be displayed after a second in textarea below",
      },
    },


    backup: {
      header: "Backup/Restore Settings",
      create: "Create Snapshot",
      view: "View Snapshot",
      open: "File to Text",
      load: "Snapshot to Text",
      restore: "Restore",
      toClipboard: "Text to Clipboard",
      tt: {
        header: `Backup: 1) Click Create Snapshot button to save a snapshot of the current settings to MCU flash. 2) Click the View Snapshot link to open settings.json and then save it externally.
Restore: 1) Paste the content of a settings.json file to the text area 2) Select parts you want to restore and click restore button`,
        create: "Save current settings to internal file settings.json (located on MCU Flash ROM)",
        open: "Open externally saved settings.json backup file and show it in text area",
        load: "Fetch the settings.json internally stored in MCU (!) and show it in the text area",
        restore: "Overwrite the MCU Settings by the JSON data in the text area",
        toClipboard: "Copy JSON from text area to clipboard",
      },
    },

    help_doc: {
      header: "Online Documentation",
      tt: {
        header: "Access documentation on GitHub",
      },
      home_page: {
        url: "https://zwiebert.github.io/tronferno-mcu",
        txt: "Online Documentation, Home-Page",
      },
      starter: {
        url: "docs/starter-de.md",
        txt: "Getting Started",
      },
      starter_faq: {
        url: "docs/starter_faq-de.md",
        txt: "Starter FAQ",
      },

      mqtt: {
        url: "docs/mqtt.md",
        txt: "MQTT Client",
      },
    },
    help_feedback: {
      header: "Report Bugs, Suggest Features, Give Feedback",
      tt: {
        header: `If you've encountered a bug, or want to suggest a new feature, the preferred of way to report/request, is by open a new "Issue" at
          the GitHub repository.
           This way other users can check, if a bug is already reported or can comment or help with a feature request.
           Please check first both repositories if there is already an open issue about this topic.
           If you don't want register an account on GitHub, then use E-Mail instead.
          `,
      },
      issues: {
        link_src: {
          info: "Feature requests and bug reports (English,German):",
          url: "https://github.com/zwiebert/tronferno-mcu/issues",
          txt: "tronferno-mcu/issues",
        },
        link_bin: {
          info: "Bug reports (English,German):",
          url: "https://github.com/zwiebert/tronferno-mcu-bin/issues",
          txt: "tronferno-mcu-bin/issues",
        },
        link_mail: {
          info: "Mail to author/maintainer (English,German):",
          url: "mailto:tf.zwiebert@online.de",
          txt: "mailto:tf.zwiebert@online.de",
        },
      },
    },

    network: { header: "Network-Connections and -Services", tt: { header: "--" } },
    misc: { header: "Misc. Settings", tt: { header: "--" } },
    cc1101: {
      header: "CC1101 RF-Module Configuration (For Developers)",
      tt: {
        header:
          "Configure CC1101 by providing a register file (ASCII-HEX string) or change a small subset of registers in the experimental configuration editor",
      },
    },

    rx_names: {
      header: "Receiver Names",
      tt: {
        header: "Edit group and receiver names",
      },
    },
    durations: {
      header: "Movement Durations",
      tt: {
        header:
          "Configure here the durations for the shutter to move from open to close and from open to sup-position. This is needed to calculate the current in-between shutter position and to reach a requested in-between position.",
      },
    },
    rot_dir: {
      header: "Motor Rotation Direction",
      tt: {
        header: `Direction of rotation. Change direction of rotation of a motor.
First check the current direction with arrow buttons. If wrong, then reverse the rotation with the button and check again`,
      },
    },

    endpos: {
      header: "End Position Setting",
      tt: {
        header: "Set lower end upper end position for Fernotron tubular-motors ",
      },
    },

    activate: {
      header: "Activate SET",
      tt: {
        header: `Activate SET-function of receiver. You have 60 seconds in which to
          register additional transmitters with the selected receiver.`,
      },
    },
    register_gm: {
      header: "Register Receiver with Programming Centre",
      tt: {
        header: `To assign a receiver to a group and give it a receiver number:
           1) active SET function of receiver. 2) transmit data (as in timer data) using the intended group/receiver-number`,
      },
    },
    register_tx: {
      header: "Register Receiver with Transmitter",
      tt: {
        header:
          "To register/unregister a transmitter to a receiver: 1) activate SET function of receiver. 2) send STOP command with transmitter -or- chose the transmitter ID here and press STOP",
      },
    },

    repeater: {
      header: "RF-Repeater",
      tt: {
        header:
          "Repeat plain RF transmitter commands by the MCU's RF transmitter to increase reach. You have to provide a list of transmitter-IDs. Only transmitters on this list will be repeated.",
      },
    },
    register: {
      header: "Register Transmitters",
      tt: {
        header: "To register position changes caused by plain controllers and sun-sensors, these controllers have to be registered here.",
      },
    },
    tx_names: {
      header: "Transmitter-Names",
      input: "TX-Name",
      tt: {
        header: "Assign names to transmitters for user convenience",
      },
    },

    hs_config: {
      home_assistant: {
        header: "Home Assistant Configuration",
        tt: {
          header: "1) Configures Home Assistant via MQTT discover or 2) Generates a configuration for Home Assistant for copy-and-paste",
        },
      },
      fhem: {
        header: "FHEM Configuration",
        tt: {
          header:
            "Generates device definitions for FHEM home-server. Definitions are generated for generic MQTT or the dedicated tronferno-fhem module. Paste the generated definitions into FHEMWEB and execute them there",
        },
      },
      openhab: {
        header: "OpenHAB Configuration",
        tt: {
          header: "Generates a configuration for OpenHAB home-server for copy-and-paste. Configurations are generated for MQTT-Generic-Thing",
        },
      },
    },

    guiacc: {
      header: "GUI Access restriction",
      tt: {
        header:
          "Experimental/Unfinished work: Reduce user level to hide inappropriate user interface areas (e.g. most configuration areas need at least admin level)",
      },
    },
  },

  app: {
    run: "Run",
    reload: "Reload",
    restartMcu: "Restart MCU",
    save: "Save",
    hide: "Hide",

    cfg: {
      mcu: {
        user_gpio: {
          add_label: "User-GPIOs",
          add_button: "Add",

          tt: {
            gpioxx: "Configure or remove a user defined GPIO here",
            add_label: "Add user defined GPIOs, which can then be configured above for input or output and later accessed via MQTT",
          },
        },
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
      sendISO: "ISO-Time",
      sendData: "Transmit Data",
      tt: {
        daily: "Daily 24h timer. One time for up/down each. You can leave one time empty if you want only one of both up or down.",
        weekly:
          "Weekly timer. Seven times for up/down each and each day of the week. If the checkbox is unchecked, then related weekday will copy the previous day's times.",
        astro: `Automatic dusk mode. (Needs longitude/latitude configured according to your location). Closes roller shutter at civil dusk. 
        You can provide an offset in minutes to adjust the closing time. Negative values will close the shutter prior to civil-dusk. Positive values after civil-dusk.`,
        random: `Automatic random operation. This setting results in a random delay of the set switching time of
        between 0 and 30 minutes. Automatic random operation does not apply for the dusk times.`,
        sun: "Automated sun-light protection mode.",
        manual:
          "Deactivate automatic operation. Disables all of the above (daily, weekly, astro, random, sun). This will overwrite the data in the receiver's storage, but a copy is kept in the MCU for later re-use.",
        sendRtc: "Transmit the current time/date to receiver or group(s).",
        sendISO: "For testing purposes only: This sends a specify time string to the receiver instead of using the actual time from internal RTC.",
        sendData: "Save data to MCU and transmit data to receiver or group. The RF transmission takes 5 seconds",
      },
    },

    id: {
      chose_header: "Pick/Enter a Transmitter-ID",
      chose_allRegIds: "Other",
      chose_rxIds: "RX",
      chose_enterId: "TX-ID",
      test_selectedId: "Test",
      send_stop: "STOP",
      regTbl_header1: "Transmitter-ID",
      regTbl_header2: "is registered to:",
      tt: {
        chose_allRegIds: "Transmitter-IDs registered to other receivers",
        chose_regIds: "Transmitter-IDs registered to this receiver number only",
        chose_rxIds: "List of unregistered transmitter-IDs received via RF",
        chose_enterId: "Enter a Transmitter-ID",
        enterName: "Enter/edit name for this transmitter",
        text_enterId: "Enter an ID or select ID from a list",
        test_selectedId: "Identify motor(s) paired with this ID by moving them",
        send_stop: "Send STOP if you want to register/unregister this transmitter with a receiver in SET-function",
        set_function: "Activate SET-function on the selected receiver number",
        register_id: "Register/Unregister selected Transmitter-ID to selected receiver-number",
        register_rf: "Register/Unregister RF-Transmitter to selected receiver-number. Click button and then press Stop on RF transmitter",
        registeredToTheseRx:
          "Shows/Edits on which Receivers a Transmitter is registered.  These registrations only mirror the real registrations in the receivers.",
      },
    },

    dur: {
      h: "Duration of Shutter-Movements",
    },

    sun: {
      move_sun_down: "\u263C\u25bc",
      set_sun_pos: "\u263C\u2699",

      tt: {
        move_sun_down: "Move shutter down to sun position (sun automatic needs to be enabled for this)",
        set_sun_pos: "Set sun position to current position",
      },
    },

    sep: {
      header: "Set End-Positions",
      enter: "Enter Set End Position Controller",
      authenticate: "Authenticate",
      auth_prog_wait: "Prepare... please wait...",
      auth_prog_press: "...press physical set-button on Tronferno-MCU device",
      enable: "Enable",
      disable: "Disable",
      exit: "Exit",

      warning_html: `
        <ul>
        <li>Use this functionality at your OWN RISK, or don't use it at all</li>
        <li>Be aware that moving a shutter beyond physical limits while adjusting end positions may harm it.</li>
        <li>Make sure you really have addressed the shutter you intend to adjust and test this with normal harmless Up/Down/Stop.</li>
        <li>Make sure only a SINGLE shutter motor is registered on the selected group/receiver number or use the motor's Radio-Code</li>
        <li>To be safe, have a independent transmitter ready to send STOP to the shutter, in case the MCU crashes in between</li>
        </ul>`,
      tt: {
        header: "Adjusting shutter end-positions. A physical button needs to be connected to MCU.",
        enter: "Open controller to adjust end positions.",
        authenticate: "Authenticate user by pressing physical button on the MCU device",
        enable: "Enable RF commands to set end positions. Shows Up and Down button you can press and hold to adjust the Up/Down end positions",
        disable: "Disable RF commands for end positions",
        exit: "Deauthenticate and leave this controller",
      },
    },

    msg_waitForMcuRestart: "Wait for MCU to restart...",
    msg_firmwareIsUpdating: "Firmware is updating...",
    msg_firmwareUpdSuccess: "Update succeeded",
    msg_enterNewName: "Shutter-Name",

    config_wizards: "Configuration-Wizards",
    wizard_cuas: "Scan Central-Unit-ID",
    direction: "Direction",
    duration: "Duration (secs)",
    stopClock: "StopClock",
    downToZero: "down to 0%",
    downSunPos: "down to sun position",
    upTo100: "up to 100%",

    rotDir: {
      toggle: "Toggle",
      test_up: "\u25b2",
      test_down: "\u25bc",
      tt: {
        toggle: "Change rotation direction of the tubular motor. Each button press reverses the direction.",
        test_up: "Test direction. Should move the shutter upwards.",
        test_down: "Test direction: Should move the shutter downwards.",
      },
    },
    setMode: {
      set_button: "Press SET-Button on Receiver.",
      set_button_radio: "Button",
      register_unregister: "Register/Unregister",
      register_cu_bt: "Register central-unit-G-E to Receiver",
      register_cu: "Central-Unit",
      register_sender: "Transmitter",
      set_mode: "SET",
      dev_address: "Device Address",
      receiver_address: "Receiver Address",
      sender_address: "Transmitter",
      gm_address: "G/E/RadioCode",
      motor_code: "Motor-Code",
      h_set_mode_enable: "Enable SET-function",
      h_register_receiver_as_gm: "Register Central-Unit to Receiver in SET-function",
      h_register_sender_to_smr: "Register a Transmitter to Receiver in SET-function",
      tt: {
        set_button: "Press here to show you how long SET function will be active.",
        set_mode: "Send RF-command to receiver to activate its SET-function",
        register_cu_bt:
          "Register the Programming Centre and group-number, receiver-number with the receiver and tell the receiver its group-/receiver-number. Will also transmit all data (like timers) to the receiver.",
        register_cu:
          "Register the Programming Centre (MCU) with the receiver. The receiver will listen to commands matching the given group-/receiver-number if they are coming from the registered Programming Centre.",
        register_sender: "Register a transmitter with the receiver. The receiver will listen to commands coming from any registered transmitter.",
      },
    },
    names: {
      restoreNames: "Restore names",
      tt: {
        restoreNames: "If MCU is new or MCU flash was erased, restore the shutter names from local browser cache storage",
      },
    },
    repeater: {
      ids: {
        header: "Transmitter(s) to be repeated",
        add_button: "Add",
        remove_button: "Remove",
        tt: {
          header:
            "List of transmitter-IDs. Only commands sent from these transmitters will be repeated via the MCU's RF-transmitter. Other transmitter's commands won't be repeated.",
          add_button: "Add",
          remove_button: "Remove",
        },
      },
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
  },
  mcuConfigNames: {
    verbose: "CLI Verbosity",
    cu: "Central-Unit-ID",
    "astro-correction": "Astro Correction",
    rtc: "Date/Time",
    "rtc-auto-upd-enable": "RTC auto updates",
    tz: "Timezone (POSIX)",
    "lan-phy": "Board or PHY-Chip",
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
    "rf-tx-pin": "RF-Transmitter",
    "rf-rx-pin": "RF-Receiver",
    "set-button-pin": "Set-Button",
    "rf-sclk-pin": "CC1101 SCLK",
    "rf-miso-pin": "CC1101 SO",
    "rf-mosi-pin": "CC1101 SI",
    "rf-ss-pin": "CC1101 CS0",
    "gm-used": "Receivers",
    cc1101: {
      "rf-sclk-pin": "CC1101 SCLK/SCK",
      "rf-miso-pin": "CC1101 MISO",
      "rf-mosi-pin": "CC1101 MOSI",
      "rf-ss-pin": "CC1101 CS0/CSN",
      "rf-tx-pin": "CC1101 GDO0",
      "rf-rx-pin": "CC1101 GDO2",
    },
  },

  mcuConfig: {
    ethernet: "LAN/Ethernet",
    wlan_station: "WLAN Station",
    ntp_client: "NTP Client",
    mqtt_client: "MQTT Client",
    http_server: "Web server",
    pin_gpio: "Electrical In-/Outputs",
    config_wizards: "Configurations-Wizards",
    gm_used: "Number of Receivers per Group",
  },

  notifyUser: {
    caption: "Firmware Changes",
    messages: [
      { date: "2022", text: "Features: Change rotation direction. Set end positions. RF-Repeater" },
      { date: "2021", text: "Hardware: Ethernet. CC1101 RF module" },
    ],
  },
};
