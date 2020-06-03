'use strict';
import * as appDebug from './app_debug.js';
import * as mcuFirmware from './mcu_firmware.svelte';
import * as cuas from './cuas.js';
import {McuConfig, Gmu} from './store/mcu_config.js';
import {Pcts,Prefs,Aliases,Autos,Names} from './store/shutters.js';
import { McuBootCount, McuGitTagNames } from "./store/mcu_firmware.js";
import {McuDocs} from './store/mcu_docs.js';



function parse_gmu(s) {


  let sa = s ? s.split('').reverse() : [];

  let gmu = [0, 1, 2, 3, 4, 5, 6, 7];

  for (let g = 1; g < 8; ++g) {
    let um = sa[g] ? parseInt(sa[g]) : 0;
    gmu[g] = um;
  }
  return gmu;
}

export function http_handleResponses(obj) {
    appDebug.dbLog("reply-json: " + JSON.stringify(obj));

    if ("config" in obj) {
      let config = obj.config;

      if ("cuas" in config) {
        cuas.cuas_handle_cuasState(config);
      } else {
        McuConfig.update(config);
        if ("gm-used" in config) {
          Gmu.set(parse_gmu(config["gm-used"]));
        }
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
      let shs = obj.shs;
      Prefs.update(shs);
      let names = {};
      let names_ct = 0;
      for (const key in shs) {
        if (key.startsWith('shs')) {
          names[key.substr(3)] = shs[key]['tag.NAME'];
          ++names_ct;
        }
      }
      if (names_ct > 0) {
        Names.update(names);
        console.log("names: ", JSON.stringify(names));
      }      
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

  export function gitTags_handleResponse(json) {
    let names = [];
    json.forEach((item) => {
      names.push(item.name);
    });
    McuGitTagNames.set(names);
  }