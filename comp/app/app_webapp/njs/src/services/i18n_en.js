export const en = {
  weekdays: ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"],
  fernotron: {
    register: "register",
    unregister: "unregister",
    Register: "Register",
    Unregister: "Unregister",
  },
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
        user_level: {
          developer: ">>",
          all: "$#",
          expert: "##>",
          admin: "#>",
          user: "$>",
          kid: "@>",
          name: {
            developer: "Developer",
            expert: "Expert",
            admin: "Admin",
            user: "User",
            kid: "Kid",
          },
        },
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
    },

    id: {
      chose_header: "Pick/Enter a Sender-ID",
      chose_allRegIds: "All",
      chose_rxIds: "RX",
      chose_enterId: "Enter",
      test_selectedId: "Test",
      regTbl_header1: "Sender-ID",
      regTbl_header2: "is registered to:",
      tt: {
        chose_allRegIds: "Any registered Sender-IDs",
        chose_regIds: "Sender-IDs registered to this receiver number only",
        chose_rxIds: "List of Sender-IDs recveived via RF",
        chose_enterId: "Enter a Sender-ID",
        text_enterId: "Enter an ID or select ID from a list",
        test_selectedId: "Identify motor(s) paired with this ID by moving them",
        register_id: "Register/Unregister selected Sender-ID to selected receiver-number",
        register_rf: "Register/Unregister RF-Sender to selected receiver-number. Click button and then press Stop on RF transmitter",
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

      warning_html: `When adjusting shutter end-postions: 
        <ul>
        <li>Use this functionality at your OWN RISK, or don't use it at all</li>
        <li>Be aware that moving a shutter beyond physical limits while adjusting end positions may harm it.</li>
        <li> Make sure you really have addressed the shutter you intend to adjust and test this with normal harmless Up/Down/Stop.</li>
        <li>Make sure only a SINGLE shutter motor is registered on the selected group/receiver number or use the motor's Radio-Code</li>
        <li>To be safe, have a indepent transmitter ready to send STOP to the shutter, in case the MCU crashes in between</li>
        </ul>`,
      tt: {
        header: "Adjusting shutter end-postions. A physical button needs to be connected to MCU.",
        enter: "Open controller to adjust end positions.",
        authenticate: "Authenticate user by pressing physical button on the MCU device",
        enable: "Enable RF commands to set end positions. Shows Up and Down button you can press and hold to adjust the Up/Down end positions",
        disable: "Disable RF commands for end postions",
        exit: "Deauthenticate and leave this controller",
      },
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

    rotDir: {
      toggle: "Toggle",
      test_up: "Test \u25b2",
      test_down: "Test \u25bc",
      tt: {
        toggle: "Change rotation direction of the tubular motor. Each button press reverses the direcion.",
        test_up: "Test direction. Should move the shutter upwards.",
        test_down: "Test direction: Should move the shutter downwards.",
      },
    },
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
      gm_address: "Central/RadioCode",
      motor_code: "Motor-Code",
      h_set_mode_enable: "Enable Set Mode",
      h_register_receiver_as_gm: "Register Central-Unit to Receiver in Set-Mode",
      h_register_sender_to_smr: "Register a Sender to Receiver in Set-Mode",
      tt: {
        tt: {
          set_mode: "Send SET command to RF-receiver to enable its set mode",
        },
      },
    },
    names: {
      restoreNames: "Restore names",
      tt: {
        restoreNames: "If MCU is new or MCU flash was erased, restore the shutter names from local browser cache storage",
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
    hint_shutterMisc: "Miscelleanous settings per shutter",
    hint_shutterDurations:
      "Configute here the durations for the shutter to move from open to close and from open to sup-position. This is needed to calculate the current in-between shutter position and to reach a requested in-between position.",
    hint_durStopClock:
      "To measure the duration of up/down movements, press Start/Stop to start both movement and clock. After the movement stops by itself, press again to stop the clock.",
    hint_shutterAlias: "To register position changes caused by plain controllers and sun-sensors, these controllers have to be registered here.",
    hint_repeater: "Repeat RF commands to increase reach. You have to provide a list of IDs. Only senders from this list will be repeated.",
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
    network: "Network",
    misc: "Other",
    ntp_client: "NTP Client",
    mqtt_client: "MQTT Client",
    http_server: "Webserver",
    pin_gpio: "Elektrical In-/Outputs",
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
