'use strict';
import * as appDebug from './app_debug.js';
import * as appState from './app_state.svelte';
import * as mcuFirmware from './mcu_firmware.svelte';
import * as cuas from './cuas.js';
import * as mcuConfig from './mcu_config.svelte';
import {McuConfig} from './store/mcu_config.js';
import {Pcts,Prefs,Aliases,Autos} from './store/shutters.js';
import {McuBootCount} from './store/mcu_firmware.js';
import {McuDocs} from './store/mcu_docs.js';

export function http_handleResponses(obj) {
    appDebug.dbLog("reply-json: " + JSON.stringify(obj));

    if ("config" in obj) {
      let config = obj.config;

      if ("cuas" in config) {
        cuas.cuas_handle_cuasState(config);
      } else {
        McuConfig.update(config);
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
      Pcts.update(obj.pct);
    }

    if ("pair" in obj) {
      const pair = obj.pair;
      if ("all" in pair) {
        Aliases.set(pair.all);
      }
      if ("a" in pair && "mm" in pair) {
        let key = pair.a;
        let val = pair.mm;
        let all = {};
        all[key] = val;
        Aliases.update(all);
      }
    }

    if ("shs" in obj) {
      Prefs.update(obj.shs);
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
        McuBootCount.set(mcu["boot-count"]);
      }
      if ("mcuFirmware-state" in mcu) {
        mcuFirmware.netota_handle_otaState(mcu["mcuFirmware-state"]);
      }

    }
    {
    let autos = {};
    let autos_count = 0;
    for (const key in obj) {
      if (key.startsWith('auto')) {
        autos[key] = obj[key];
        ++autos_count;
      }
    }
    if (autos_count) {
      Autos.update(autos);
    }
    }
  }

  export function http_handleDocResponses(name, text) {
    let obj = {};
    obj[name] = { 'text': text };
    McuDocs.update(obj);
    
  }