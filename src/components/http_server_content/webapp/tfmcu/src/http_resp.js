'use strict';
import * as appDebug from './app_debug.js';
import * as appState from './app_state.js';
import * as mcuFirmware from './mcu_firmware.svelte';
import * as cuas from './cuas.js';
import * as mcuConfig from './mcu_config.svelte';

export function http_handleResponses(obj) {
    appDebug.dbLog("reply-json: " + JSON.stringify(obj));

    if ("config" in obj) {
      let config = obj.config;

      if ("cuas" in config) {
        cuas.cuas_handle_cuasState(config);
      } else {
        appState.ast.tfmcu_config = Object.assign(appState.ast.tfmcu_config, config);
        if ("gm-used" in config) {
          mcuConfig.usedMembers_fromConfig();
        }

        if (!document.getElementById("cfg_table_id")) {
          if ("verbose" in config) {
            document.getElementById("config-div").innerHTML = mcuConfig.mcuConfigTable_genHtml(obj.config);
          }
        }
      }

      if (document.getElementById("cfg_table_id")) {
        mcuConfig.mcuConfig_updHtml(obj.config);
        mcuConfig.usedMembers_updHtml_fromHtml();
      }
    }

    if ("pct" in obj) {
      appState.ast.pcts = obj.pct;
    }

    if ("pair" in obj) {
      const pair = obj.pair;
      if ("all" in pair) {
        appState.ast.aliases = pair.all;
      }
      if ("a" in pair && "mm" in pair) {
        appState.ast.aliases_add(pair.a, pair.mm);
      }

    }

    if ("shs" in obj) {
      const shs = obj.shs;
      appState.ast.shutterPrefs = shs;
    }

    if ("mcu" in obj) {
      let mcu = obj.mcu;
      if ("chip" in mcu) {
        document.getElementById("id_chip").innerHTML = mcu.chip;
      }
      if ("firmware" in mcu) {
        document.getElementById("id_firmware").innerHTML = mcu.firmware;
      }
      if ("build-time" in mcu) {
        document.getElementById("id_buildTime").innerHTML = mcu["build-time"];
      }
      if ("boot-count" in mcu) {
        appState.ast.mEsp32BootCount = mcu["boot-count"];
        appState.ast.updateHtml_bootCount();
      }
      if ("mcuFirmware-state" in mcu) {
        mcuFirmware.netota_handle_otaState(mcu["mcuFirmware-state"]);
      }

    }

    if (appState.ast.getAutoName() in obj) {
      appState.ast.auto = obj[appState.ast.getAutoName()];
    }
  }