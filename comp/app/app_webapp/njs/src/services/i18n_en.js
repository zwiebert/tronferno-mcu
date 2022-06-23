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
        firmware: "OTA",
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
        sender: "Transmitter",
        receiver: "Receiver",
        rot_dir: "Rotation-Direction",
        endpos: "End-Positions",
        set: "SET",
        positions: "Positions",
        set_mode: "SET",
        app: "App",
      },
      sender: {
        aliases: "Aliases",
        repeater: "RF-Repeater",
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

      receiver: {
        tab_name: "Receiver",
        rot_dir: {
          tab_name: "Rotation",
        },
        end_pos: {
          tab_name: "End-Position",
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
      chose_header: "Pick/Enter a Transmitter-ID",
      chose_allRegIds: "All",
      chose_rxIds: "RX",
      chose_enterId: "Enter",
      test_selectedId: "Test",
      regTbl_header1: "Transmitter-ID",
      regTbl_header2: "is registered to:",
      tt: {
        chose_allRegIds: "Any registered Transmitter-IDs",
        chose_regIds: "Transmitter-IDs registered to this receiver number only",
        chose_rxIds: "List of Transmitter-IDs recveived via RF",
        chose_enterId: "Enter a Transmitter-ID",
        text_enterId: "Enter an ID or select ID from a list",
        test_selectedId: "Identify motor(s) paired with this ID by moving them",
        register_id: "Register/Unregister selected Transmitter-ID to selected receiver-number",
        register_rf: "Register/Unregister RF-Transmitter to selected receiver-number. Click button and then press Stop on RF transmitter",
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
      set_button: "Press SET-Button on Receiver first. Then click here.",
      set_button_radio: "Button",
      register_unregister: "Register/Unregister",
      register_cu_bt: "Register central-unit-G-E to Receiver",
      register_cu: "Central-Unit",
      register_sender: "Transmitter",
      set_mode: "Enable SET-function",
      dev_address: "Device Address",
      receiver_address: "Receiver Address",
      sender_address: "Transmitter",
      gm_address: "G/E/RadioCode",
      motor_code: "Motor-Code",
      h_set_mode_enable: "Enable SET-function",
      h_register_receiver_as_gm: "Register Central-Unit to Receiver in SET-function",
      h_register_sender_to_smr: "Register a Transmitter to Receiver in SET-function",
      tt: {
        set_mode: "Send RF-command to receiver to activate its SET-function",
        register_cu_bt: "Register the Programming Centre and group-number, receiver-number with the receiver and tell the receiver its group-/receiver-number. Will also transmit all data (like timers) to the receiver.",
        register_cu: "Register the Programming Centre (MCU) with the receiver. The receiver will listen to commands matching the given group-/receiver-number if they are coming from the registered Programming Centre.",
        register_sender: "Register a transmitter with the receiver. The receiver will listen to commands coming from any registered transmitter.",
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
    "rf-tx-pin": "RF-Transmitter",
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
    misc: "General",
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
