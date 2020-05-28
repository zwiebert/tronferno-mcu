'use strict';
import { dbLog } from './misc.js';
import * as misc from './misc.js';
import * as shutterName from './shutter_name.js';
import * as shutterPrefs from './shutter_prefs.js';
import * as a_http from './fetch.js';
import * as ota from './netota.js';
import * as cuas from './cuas.js';
import * as mcc from './mcu_config.js';

export let ast;



export function init() {
  ast = new AppState();
  ast.load();
}

export class AppState {

  constructor() {
    this.mG = Number.parseInt((localStorage.getItem("group") || "0"), 10);
    this.mM = Number.parseInt((localStorage.getItem("member") || "0"), 10);
    this.mGmu = localStorage.getItem("gmu") || [0, 1, 2, 3, 4, 5, 6, 7];
    this.mTabVisibility = Number.parseInt((localStorage.getItem("tab_vis") || "1"), 10);
    this.mAuto = { link: {}, auto: {} };
    this.mPair = { all: {} };
    this.mShutterPrefs = {};
    this.mPct = 0;
    this.mPcts = {};
    this.docs = {};
    this.tfmcu_config = {};
    this.gmc_fetch = 0;
    this.load_fetch = a_http.FETCH_GMU;
    this.mEsp32BootCount = 0;
    this.mWebSocket = 0;
  }

  websocket() {
    this.mWebSocket = new WebSocket('ws://' + window.location.host + '/ws');
    this.mWebSocket.onopen = (evt) => { this.mWebSocket.send(JSON.stringify({ "to": "tfmcu", "cmd": { "p": "?" } })); };
    this.mWebSocket.onmessage = (evt) => { let json = evt.data; let obj = JSON.parse(json); this.http_handleResponses(obj); };
    this.mWebSocket.onclose = (evt) => { dbLog(evt.reason); setTimeout(function() { this.websocket(); }, 1000); };
    this.mWebSocket.onerror = (err) => { dbLog(err.msg); this.mWebSocket.close(); };
  }

  load() {
    gm_updHtml();
    this.tabIdx = this.mTabIdx;
    a_http.http_fetchByMask(this.load_fetch);
    this.tabVisibility = this.mTabVisibility;
    setTimeout(() => { ast.websocket(); }, 1000);
    //this.websocket();
  }

  set tabVisibility(value) {
    this.mTabVisibility = value;
    localStorage.setItem("tab_vis", value.toString());
    misc.navTabs_updHtml(parseInt(value));
  }

  get tabVisibility() {
    return this.mTabVisibility;
  }

  set pct(val) {
    this.mPct = val;
    document.getElementById('spi').value = val;
    document.getElementById('spr').value = val;
  }

  get pct() {
    return this.mPct;
  }

  set pcts(obj) {
    Object.assign(this.mPcts, obj);
    let key = this.g.toString() + this.m.toString();
    if (key in obj) {
      this.pct = obj[key];
    }
  }

  get pcts() {
    return this.mPcts;
  }

  getAutoName() { return "auto" + this.g.toString() + this.m.toString(); }

  getAutoObj() { let key = this.getAutoName(); return (key in this.mAuto.auto) ? this.mAuto.auto[key] : {}; }

  linkAutoObj() { this.mAuto.link = this.getAutoObj(); }

  gmChanged() {
    this.pct = this.pcts[this.g.toString() + this.m.toString()];
    shutterName.shn_updHtml();
    this.linkAutoObj();
    this.automaticOptions_updHtml();
    dbLog(JSON.stringify(this));
    if (this.gmc_fetch)
      a_http.http_fetchByMask(this.gmc_fetch);
    misc.aliasPaired_updHtml();
  }

  get g() {
    return this.mG;
  }

  g_next() {
    for (let i = this.mG + 1; i <= 7; ++i) {
      if (this.mGmu[i] > 0) {
        this.g = i;
        return;
      }
    }
    this.g = 0;
  }

  m_next() {
    this.m = (this.mM >= this.gmu[this.g]) ? 0 : this.mM + 1;
  }

  set g(value) {
    this.mG = value;
    localStorage.setItem("group", value.toString());
    if (value != 0) {
      if (this.mM > this.gmu[value]) {
        value = this.gmu[value];
        this.mM = value;
        localStorage.setItem("member", value.toString());
      }
    }

    gm_updHtml();
    this.gmChanged();
  }

  get m() {
    return this.mG === 0 ? 0 : this.mM;
  }

  set m(value) {
    if (this.mG !== 0) {
      this.mM = value;
      localStorage.setItem("member", value.toString());
      document.getElementById("smi").value = value ? value : "A";
      gm_updHtml();
      this.gmChanged();
    }
  }

  get gmu() {
    return this.mGmu;
  }

  set gmu(value) {
    this.mGmu = value;
    localStorage.setItem("gmu", value);
  }

  get auto() {
    return this.mAuto.link;
  }

  set auto(obj) {
    this.mAuto.auto[this.getAutoName()] = obj;
    this.linkAutoObj();
    this.automaticOptions_updHtml();
  }

  aliases_add(key, val) {
    this.mPair.all[key] = val;
    misc.aliasControllers_updHtml();
  }

  set aliases(value) {
    this.mPair.all = value;
    misc.aliasControllers_updHtml();
  }

  get aliases() {
    return this.mPair.all;
  }

  set shutterPrefs(obj) {
    Object.assign(this.mShutterPrefs, obj);
    shutterPrefs.shp_updHtml();
    shutterName.shn_updHtml();
  }

  get shutterPrefs() {
    return this.mShutterPrefs;
  }

  automaticOptions_updHtml() {
    let auto = this.auto;
    document.getElementById('tfti').value = ("f" in auto) ? auto.f : "";

    document.getElementById('tati').value = ("astro" in auto) ? auto.astro : "";
    document.getElementById('twti').value = ("weekly" in auto) ? auto.weekly : "";

    let f = ("f" in auto) ? auto.f : "";
    document.getElementById('tdci').checked = f.indexOf("D") >= 0;
    document.getElementById('twci').checked = f.indexOf("W") >= 0;
    document.getElementById('taci').checked = f.indexOf("A") >= 0;
    document.getElementById('trci').checked = f.indexOf("R") >= 0;
    document.getElementById('tsci').checked = f.indexOf("S") >= 0;
    document.getElementById('tmci').checked = f.indexOf("M") >= 0;

    let up_elem = document.getElementById('tduti');
    let down_elem = document.getElementById('tddti');
    up_elem.value = "";
    down_elem.value = "";

    if ("daily" in auto) {
      let d = auto.daily;
      let l = auto.daily.length;
      up_elem.value = d.startsWith("-") ? "" : d.substring(0, 2) + ":" + d.substring(2, 4);
      down_elem.value = d.endsWith("-") ? "" : d.substring(l - 4, l - 2) + ":" + d.substring(l - 2);
    }
    if ("asmin" in auto) {
      document.getElementById("id_astroTime").innerHTML = "(today: " + Math.floor((auto.asmin / 60)).toString().padStart(2, '0') + ":" + (auto.asmin % 60).toString().padStart(2, '0') + ")";
    } else {
      document.getElementById("id_astroTime").innerHTML = "";
    }

  }

  http_handleResponses(obj) {
    dbLog("reply-json: " + JSON.stringify(obj));

    if ("config" in obj) {
      let config = obj.config;

      if ("cuas" in config) {
        cuas.cuas_handle_cuasState(config);
      } else {
        this.tfmcu_config = Object.assign(this.tfmcu_config, config);
        if ("gm-used" in config) {
          mcc.usedMembers_fromConfig();
        }

        if (!document.getElementById("cfg_table_id")) {
          if ("verbose" in config) {
            document.getElementById("config-div").innerHTML = mcc.mcuConfigTable_genHtml(obj.config);
          }
        }
      }

      if (document.getElementById("cfg_table_id")) {
        mcc.mcuConfig_updHtml(obj.config);
        mcc.usedMembers_updHtml_fromHtml();
      }
    }

    if ("pct" in obj) {
      this.pcts = obj.pct;
    }

    if ("pair" in obj) {
      const pair = obj.pair;
      if ("all" in pair) {
        this.aliases = pair.all;
      }
      if ("a" in pair && "mm" in pair) {
        this.aliases_add(pair.a, pair.mm);
      }

    }

    if ("shs" in obj) {
      const shs = obj.shs;
      this.shutterPrefs = shs;
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
        this.mEsp32BootCount = mcu["boot-count"];
        this.updateHtml_bootCount();
      }
      if ("ota-state" in mcu) {
        ota.netota_handle_otaState(mcu["ota-state"]);
      }

    }

    if (this.getAutoName() in obj) {
      this.auto = obj[this.getAutoName()];
    }
  }

  updateHtml_bootCount() {
    document.getElementById("id-bootCount").innerHTML = this.mEsp32BootCount.toString();
  }

  http_handleDocResponses(name, text) {
    this.docs[name] = { 'text': text };
  }
}

function gm_updHtml() {
  const g = ast.g;
  const m = ast.m;
  document.getElementById("sgi").value = g ? g : "A";
  document.getElementById("smi").value = g ? (m ? m : "A") : "";
}


