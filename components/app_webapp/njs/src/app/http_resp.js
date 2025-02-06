"use strict";
import * as httpFetch from "../app/fetch.js";
import * as appDebug from "../app/app_debug.js";
import { RfCapture } from "../store/rf.js";
import {
  McuBootCount,
  McuErrorBits,
  McuGitTagNames,
  McuFirmwareBuildDate,
  McuChipId,
  McuFirmwareVersion,
  McuFirmwareUpdProgress,
  McuFirmwareUpdState,
} from "../store/mcu_firmware.js";
import { TxNames } from "../store/id.js";
import * as cuas from "../app/cuas.js";
import { McuSettings, McuConfig, Gmu, Cc1101Config, Cc1101Status, Backup, Cuas } from "../store/mcu_config.js";
import { Pcts, Prefs, Aliases, Autos, Names } from "../store/shutters.js";
import { McuDocs } from "../store/mcu_docs.js";
import { Gpios } from "../store/gpio.js";
import { Sep } from "../store/sep";
import { AppLog } from "../store/app_log.js";
import { Pras, ReceivedAddresses } from "../store/alias.js";

function parse_gmu(s) {
  let sa = s ? s.split("").reverse() : [];

  let gmu = [0, 1, 2, 3, 4, 5, 6, 7];

  for (let g = 1; g < 8; ++g) {
    let um = sa[g] ? parseInt(sa[g]) : 0;
    gmu[g] = um;
  }
  return gmu;
}

export function http_handleResponses(obj) {
  appDebug.dbLog("reply-json: " + JSON.stringify(obj));

  if ("mcu" in obj) {
    if ("settings" in obj.mcu) {
      McuSettings.set(obj.mcu.settings);
      handle_settings({ ...obj.mcu.settings });
    }
  }

  if ("log" in obj) {
    AppLog.update((old) => {
      old.unshift({ log: obj.log, date: new Date() });
      return old;
    });
  }

  if ("rc" in obj) {
    AppLog.update((old) => {
      old.unshift({ rc: obj.rc, date: new Date() });
      return old;
    });
    ReceivedAddresses.update((old) => {
      old.add(obj.rc.a);
      return old;
    });
  }

  if ("sc" in obj) {
    AppLog.update((old) => {
      old.unshift({ sc: obj.sc, date: new Date() });
      return old;
    });
  }

  if ("backup" in obj) {
    Backup.set(obj.backup);
  }

  if ("config" in obj) {
    handle_config(obj.config);
  }
  
  if ("cuas" in obj) {
    Cuas.set(obj.cuas);
  }


  if ("pct" in obj) {
    Pcts.update(obj.pct);
  }

  if ("pair" in obj) {
    handle_pair(obj.pair);
  }

  if ("pras" in obj) {
    let pras = obj.pras;
    Pras.set(pras);
    if (pras.success) {
      httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES);
    }
  }

  if ("kvs" in obj) {
    handle_kvs(obj.kvs);
  }

  if ("shs" in obj) {
    handle_shs(obj.shs);
  }
  if ("sep" in obj) {
    Sep.update(obj.sep);
  }

  if ("mcu" in obj) {
    let mcu = obj.mcu;

    if ("chip" in mcu) {
      McuChipId.set(mcu.chip);
    }
    if ("firmware" in mcu) {
      McuFirmwareVersion.set(mcu.firmware);
    }
    if ("build-time" in mcu) {
      McuFirmwareBuildDate.set(mcu["build-time"]);
    }
    if ("boot-count" in mcu) {
      McuBootCount.set(mcu["boot-count"]);
    }
    if ("error-bits" in mcu) {
      McuErrorBits.set(mcu["error-bits"]);
    }
    if ("rf-capture" in mcu) {
      RfCapture.set(mcu["rf-capture"]);
    }
    if ("cc1101-status" in mcu) {
      Cc1101Status.set(mcu["cc1101-status"]);
    }
    if ("cc1101-config" in mcu) {
      Cc1101Config.set(mcu["cc1101-config"]);
    }

    if ("ota-state" in mcu) {
      let ota_state = mcu["ota-state"];

      McuFirmwareUpdState.set(Number.parseInt(ota_state));
      handleOtaState(ota_state);
    }

    for (const key in obj.mcu) {
      if (key.startsWith("gpio")) {
        let gpio = {};
        gpio[key] = obj.mcu[key];
        Gpios.update(gpio);
      }
    }
  }

  if ("auto" in obj) {
    handle_auto_obj(obj.auto);
  }
}

export function http_handleDocResponses(name, text) {
  //console.log("http_handleDocResponses:", name, text);
  let obj = {};
  obj[name] = { text: text };
  McuDocs.update(obj);
}

export function gitTags_handleResponse(json) {
  let names = [];
  json.forEach((item) => {
    names.push(item.name);
  });
  McuGitTagNames.set(names);
}

function handleOtaState(state) {
  if (state === 1) McuFirmwareUpdProgress.update((old) => old + 1);
  else McuFirmwareUpdProgress.set(0);
}

function handle_kvs(kvs) {
  let names = {};
  for (const key in kvs) {
    if (key.startsWith("TXN.")) {
      names[key.substring(4)] = kvs[key];
    }
  }
  if (names) {
    TxNames.update(names);
  }
}

function handle_config(config) {
  if ("cuas" in config) {
    cuas.cuas_handle_cuasState(config);
  } else {
    McuConfig.update(config);
    if ("gm-used" in config) {
      Gmu.set(parse_gmu(config["gm-used"]));
    }
  }
}

function handle_pair(pair) {
  if ("all" in pair) {
    Aliases.set(pair.all);
  }
  if ("a" in pair && "mm" in pair) {
    let key = pair.a;
    let val = pair.mm;
    let all = {};
    all[key] = { ...val };
    Aliases.update(all);
  }
}

function handle_auto_array(auto) {
  let tmp = {};
  for (const el of auto) {
    if ("g" in el && "m" in el) {
      const key = "auto" + el.g.toString() + el.m.toString();
      tmp[key] = { ...el };
      delete tmp[key].g;
      delete tmp[key].m;
    }
  }

  Autos.update(tmp);
}

function handle_auto_obj(auto) {
  let tmp = {};

  for (const key in auto) {
    if (key.startsWith("auto")) {
      tmp[key] = { ...auto[key] };
    }
  }

  Autos.update(tmp);
}

function handle_shs(shs) {
  Prefs.update(shs);
  let names = {};
  let names_ct = 0;
  for (const key in shs) {
    if (key.startsWith("shs") && "tag.NAME" in shs[key]) {
      names[key.substring(3)] = shs[key]["tag.NAME"];
      ++names_ct;
    }
  }
  if (names_ct > 0) {
    Names.update(names);
    console.log("names: ", JSON.stringify(names));
  }
}
//TODO: add the other properties like in handle_shs()
function handle_shpref_array(shpref) {
  let names = {};
  for (const el of shpref) {
    if ("tag.NAME" in el && "g" in el && "m" in el) {
      const key = el.g.toString() + el.m.toString();
      names[key] = el["tag.NAME"];
    }
  }
  Names.update(names);
}
function handle_settings(settings) {
  if ("shpref" in settings) {
    handle_shpref_array(settings.shpref);
  }

  if ("config" in settings) {
    handle_config(settings.config);
  }

  if ("pair" in settings) {
    handle_pair(settings.pair);
  }

  if ("auto" in settings) {
    handle_auto_array(settings.auto);
  }
}
