var base = '';
var gmu = [0,1,2,3,4,5,6,7];
var gu = [0,1,2,3,4,5,6,7];
var gu_idx = 0;

let cuas_Interval;
let cuas_State = 0;
let gmc_fetch = 0;

const FETCH_CONFIG = 1;
const FETCH_AUTO = 2;
const FETCH_POS = 4;
const FETCH_VERSION = 8;
const FETCH_ALIASES = 16;
const FETCH_ALIASES_START_PAIRING = 32;
const FETCH_ALIASES_START_UNPAIRING = 64;
const FETCH_SHUTTER_PREFS = 128;
const FETCH_GMU = 256;

const DIR_UP = 0;
const DIR_DOWN = 1;

function dbLog(msg) {
  console.log(msg);
}

class AppState {

  constructor() {
    this.mG = Number.parseInt((localStorage.getItem("group") || "0"), 10);
    this.mM = Number.parseInt((localStorage.getItem("member") || "0"), 10);
    this.mTabVisibility = Number.parseInt((localStorage.getItem("tab_vis") || "1"), 10);
    this.mAuto = {link:{}, auto:{}};
    this.mPair = {all:{}};
    this.mShutterPrefs = {};
    this.mPct = 0;
    this.docs = {};
    this.tfmcu_config = {};
  }

  set tabVisibility(value) {
    this.mTabVisibility = value;
    localStorage.setItem("tab_vis", value.toString());
    navTabs_updHtml(parseInt(value));
  }
  get tabVisibility() {
    return  this.mTabVisibility;
  }

  get g() {
    return this.mG;
  }

  set pct(val) {
    this.mPct = val;
    document.getElementById('spi').value = val;
    document.getElementById('spr').value = val;
    shuPos_perFetch_posRecieved(val);
  }
  get pct() {
    return this.mPct;
  }
  getAutoName() { return  "auto" + this.g.toString() + this.m.toString(); }
  getAutoObj() { let key = this.getAutoName(); return (key in this.mAuto.auto) ? this.mAuto.auto[key] : {}; }
  linkAutoObj() { this.mAuto.link = this.getAutoObj(); }
  gmChanged() {
    this.linkAutoObj();
    this.automaticOptions_updHtml();
    dbLog(JSON.stringify(this));
    if (gmc_fetch)
      this.http_fetchByMask(gmc_fetch);
    aliasPaired_updHtml();
  }

  set g(value) {
    this.mG = value;
    localStorage.setItem("group", value.toString());
    document.getElementById("sgi").value = value ? value : "A";

    if (value === 0) {
      document.getElementById("smi").value = "";
    } else {
      if (this.mM > gmu[value]) {
        value = gmu[value];
        this.mM = value;
        localStorage.setItem("member", value.toString());
        document.getElementById("smi").value = value ? value : "A";
      } else {
        document.getElementById("smi").value = this.mM ? this.mM : "A";
      }
    }
    this.gmChanged();
  }

  get m() {
    return this.mG === 0 ? 0 : this.mM;
  }

  set m(value) {
    if (this.mG == 0)
      return;
    this.mM = value;
    localStorage.setItem("member", value.toString());
    document.getElementById("smi").value = value ? value : "A";
    this.gmChanged();
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
    aliasControllers_updHtml();
  }
  set aliases(value) {
    this.mPair.all = value;
    aliasControllers_updHtml();
  }
  get aliases() {
    return  this.mPair.all;
  }
  set shutterPrefs(obj) {
    Object.assign(this.mShutterPrefs, obj);
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
      up_elem.value = d.startsWith("-") ? "" : d.substring(0,2)+":"+d.substring(2,4);
      down_elem.value = d.endsWith("-") ? "" : d.substring(l-4,l-2)+":"+d.substring(l-2);
    }
    if ("asmin" in auto) {
      document.getElementById("id_astroTime").innerHTML = "(today: "+ Math.floor((auto.asmin/60)).toString().padStart(2,'0') + ":" + (auto.asmin%60).toString().padStart(2,'0') + ")";
    } else {
      document.getElementById("id_astroTime").innerHTML = "";
    }

  }

  http_handleResponses(obj) {
    dbLog("reply-json: "+JSON.stringify(obj));

    if ("config" in obj) {
      let config = obj.config;

      if ("cuas" in config) {
        if (config.cuas != cuas_State) {
          let s = "";
          switch(config.cuas) {
          case 0: s = ""; break;
          case 1: s = '<strong style="animation: blink .75s linear 4;"> ...Scanning...(press STOP on Central Unit) </strong>'; break;
          case 2: s = '<strong style="background-color:red;">Time-Out! (no STOP-command received)</strong>'; break;
          case 3: s = '<strong style="background-color:green;">Success (cu-id has been saved)</strong>';
            this.http_fetchByMask(FETCH_CONFIG);
            break;
          }

          if (config.cuas > 1) {
            window.clearInterval(cuas_Interval);
          }
          document.getElementById("id_cuasStatus").innerHTML = s;
          config.cuas = cuas_State;
        }
      } else if (document.getElementById("cfg_table_id")) {
        mcuConfig_updHtml(obj.config);
        this.tfmcu_config = obj;
        usedMembers_updHtml();
      } else if ("gm-used" in config && !("cu" in config)) {
        this.tfmcu_config = Object.assign(this.tfmcu_config, config);
        usedMembers_fromConfig();
        console.log("tfmcu_config", this.tfmcu_config);
      } else {
        document.getElementById("config-div").innerHTML = mcuConfigTable_genHtml(obj.config);
        mcuConfig_updHtml(obj.config);
        this.tfmcu_config = obj;
        usedMembers_updHtml();
      }
    }

    if ("position" in obj) {

      this.pct = obj.position.p;
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

    if ("shpref" in obj) {
      this.shutterPrefs = obj.shpref;
      shutterPrefs_updHtml();

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
      if ("ota-state" in mcu) {
        let e = document.getElementById("netota_progress_div");
        switch(mcu["ota-state"]) {
        case 0: // none

          break;
        case 1: // run
          document.getElementById("netota_progress_bar").value = (++netota_progressCounter).toString();
          break;
        case 2: // fail
          e.innerHTML += "<br><strong>Update failed<br><br></strong>";
          break;
        case 3: // done
          document.getElementById("netota_progress_bar").value = document.getElementById("netota_progress_bar").max;
          e.innerHTML += '<br><strong>Update succeeded <button type="button" onClick="req_mcuRestart()">Reboot MCU</button><br><br></strong>';
          break;
        }
        if (netota_isInProgress && mcu["ota-state"] != 1) {
          clearInterval(netota_intervalID);
          netota_progressCounter = 0;
          netota_isInProgress = false;
          document.getElementById("netota_controls").style.display = "";
        }

      }

    }

    if (this.getAutoName() in obj) {
      this.auto = obj[this.getAutoName()];
    }
  }

  http_handleDocResponses(name, text) {
    this.docs[name] =  { 'text':text };
  }

  http_fetchByMask(mask) {
    let tfmcu = {to:"tfmcu"};

    if (mask & FETCH_CONFIG)
      tfmcu.config = { all: "?" };

    if (mask & FETCH_GMU)
      tfmcu.config = { 'gm-used': "?" };

    if (mask & FETCH_VERSION)
      tfmcu.mcu = { version:"?" };


    if (mask & FETCH_AUTO)
      tfmcu.auto = {
        g: this.g,
        m: this.m,
        f: "uki",
      };

    if (mask & FETCH_POS)
      tfmcu.send = {
        g: this.g,
        m: this.m,
        p: "?",
      };

    if (mask & FETCH_ALIASES)
      tfmcu.pair = {
        c: "read_all"
      };

    if (mask & FETCH_ALIASES_START_PAIRING)
      tfmcu.pair = {
        a: "?",
        g: this.g,
        m: this.m,
        c: "pair"
      };
    if (mask & FETCH_ALIASES_START_UNPAIRING)
      tfmcu.pair = {
        a: "?",
        g: this.g,
        m: this.m,
        c: "unpair"
      };

    if (mask & FETCH_SHUTTER_PREFS)
      tfmcu.shpref = {
        g: this.g,
        m: this.m,
        c: "read",
      };

    let url = base+'/cmd.json';
    http_postRequest(url, tfmcu);
  }

  load() {
    document.getElementById("sgi").value = this.mG ? this.mG : "A";
    document.getElementById("smi").value = this.mM ? this.mM : "A";
    this.tabIdx = this.mTabIdx;
    this.http_fetchByMask(FETCH_GMU);
    this.tabVisibility = this.mTabVisibility;
  }

}

let ast;

function shutterPrefs_updHtml() {
  const key = "shp" + ast.g.toString() + ast.m.toString();
  let pref = ast.shutterPrefs[key];
  const mvut = document.getElementById("shpMvut");
  const mvdt = document.getElementById("shpMvdt");

  if (!pref) {
    pref = {mvut:0, mvdt:0};
  }

  if (pref) {
    mvut.value = (parseFloat(pref.mvut) / 10.0).toString();
    mvdt.value = (parseFloat(pref.mvdt) / 10.0).toString();
  }
}

function shutterPrefs_fromHtml_toMcu() {
  const mvut = document.getElementById("shpMvut");
  const mvdt = document.getElementById("shpMvdt");


  let tfmcu = {"to":"tfmcu", "shpref":{"g":ast.g, "m":ast.m, "c":"store"}};
  let pref = tfmcu.shpref;

  pref.mvut = Math.floor((parseFloat(mvut.value) * 10)).toString();
  pref.mvdt = Math.floor((parseFloat(mvdt.value) * 10)).toString();

  var url = base+'/cmd.json';
  http_postRequest(url, tfmcu);
}


let shutterPrefs_stopClock = {
  ms: 100,
  direction:0,
  ivId:0,
  text_elem:null,
  val:0,
};

function shutterPrefs_stopClock_tick() {
  let spsc = shutterPrefs_stopClock;
  let elem = document.getElementById(spsc.direction == DIR_UP ? "shpMvut": "shpMvdt");

  spsc.val  += (spsc.ms / 100);
  elem.value = (spsc.val / 10.0).toString();
}

function shutterPrefs_stopClock_start() {
  let spsc = shutterPrefs_stopClock;

  const pct = ast.pct;
  if (pct === 0) {
    spsc.direction = DIR_UP;
    http_postShutterCommand('up');
  } else if (pct === 100) {
    spsc.direction = DIR_DOWN;
    http_postShutterCommand('down');
  } else {
    return;
  }

  spsc.val = 0;


  if (!spsc.ivId) {
    spsc.ivId = setInterval(shutterPrefs_stopClock_tick, spsc.ms);
  }
}

function shutterPrefs_stopClock_stop() {
  let spsc = shutterPrefs_stopClock;
  http_postShutterCommand('stop');
  if (spsc.ivId) {
    clearInterval(spsc.ivId);
    spsc.ivId = 0;
    ast.pct = spsc.direction == DIR_UP ? 100 : 0;
  }
}

function aliasControllers_updHtml() {
  if (document.getElementById("divPairAll").innerHTML.length < 20) {
    document.getElementById("divPairAll").innerHTML = aliasTable_genHtml();
  }

  const pad = ast.aliases;
  const pas =  document.getElementById("aliases");
  const pas_sel = pas.selectedIndex;
  for(const key in pad) {
    let exist = false;
    for (let opt of pas.options) {
      if (opt.text == key) {
        exist = true;
      }
    }
    if (!exist) {
      let option = document.createElement("option");
      option.text = key;
      pas.add(option);
    }
  }
  if (pas.options.length > 0) {
    pas.selectedIndex = (pas_sel && pas_sel > 0) ? pas_sel : "0";
    onAliasesChanged();
  }
  aliasPaired_updHtml();
}

function aliasControllers_fromHtml() { // XXX
  const pad = ast.aliases;
  const pas =  document.getElementById("aliases");
  const pas_sel = pas.selectedIndex;

  for (let opt of pas.options) {
    const key = opt.text;
    aliasTable_fromHtml(key);
  }
}

const shuPos_perFetch = {
  ivId: 0,
  ms: 1000,
  last_pct: 0,
  last_last_pct: 0,
};

function shuPos_perFetch_posRecieved(pct) {
  const sppf = shuPos_perFetch;
  if (sppf.ivId) {
    sppf.last_last_pct = sppf.last_pct;
    sppf.last_pct = pct;
  }
}

function shuPos_perFetch_tick() {
  const sppf = shuPos_perFetch;
  const pct = ast.pct;
  if ((ast.g == 0 || ast.m == 0) || (pct === sppf.last_pct && sppf.last_pct === sppf.last_last_pct)) {
    clearInterval(sppf.ivId);
    sppf.ivId = 0;
    return;
  }
  ast.http_fetchByMask(FETCH_POS);
}

function shuPos_perFetch_start() {
  if (ast.g == 0 || ast.m == 0)
    return;

  let sppf = shuPos_perFetch;
  if (!sppf.ivId)   {
    sppf.ivId = setInterval(shuPos_perFetch_tick, sppf.ms);
    sppf.last_pct = -1;
    sppf.last_last_pct = -2;
    ast.http_fetchByMask(FETCH_POS);
  }
}

function onAliasesChanged() {
  const pas =  document.getElementById("aliases");
  if (pas.selectedIndex >= 0) {
    const key = pas.options[pas.selectedIndex].text;
    document.getElementById("paired").value = key;
    aliasTable_updHtml(key);
  }
}

function onPairedChanged() {
  let pas =  document.getElementById("paired");
  if (pas.selectedIndex >= 0) {
    const key = pas.options[pas.selectedIndex].text;
    document.getElementById("aliases").value = key;
    aliasTable_updHtml(key);
  }
}


function onAliasesApply() {
  const pas =  document.getElementById("aliases");
  if (pas.selectedIndex >= 0) {
    const key = pas.options[pas.selectedIndex].text;
    aliasTable_fromHtml_toMcu(key);
  }
}
function onAliasesReload() {
  ast.http_fetchByMask(FETCH_ALIASES);
}

function alias_isKeyPairedToM(key, g, m) {
  const val = ast.aliases[key];

  let chunks = [];

  for (let i = 0, charsLength = val.length; i < charsLength; i += 2) {
    chunks.unshift(val.substring(i, i + 2));
  }

  const g_max = chunks.length - 1;

  if (g > g_max)
    return false;

  const b = parseInt(chunks[g],16);
  console.log("b", b);

  return (b & (1 << m)) != 0;

}

function aliasPaired_updHtml() {
  const g = ast.g;
  const m = ast.m;
  const pas =  document.getElementById("paired");

  for(let i=pas.options.length-1; i >= 0; --i)
    pas.remove(i);
  console.log(ast.aliases);
  for (let key in ast.aliases) {
    console.log("key",key);
    if (!alias_isKeyPairedToM(key, g, m))
      continue;

    let option = document.createElement("option");
    option.text = key;
    pas.add(option);
  }
}
function aliasTable_updHtml(key){
  const val = ast.aliases[key];

  let chunks = [];

  for (let i = 0, charsLength = val.length; i < charsLength; i += 2) {
    chunks.unshift(val.substring(i, i + 2));
  }

  const g_max = chunks.length - 1;

  for(let g=1; g <= 7; ++g) {

    const gn = (g > g_max) ? 0 : parseInt(chunks[g], 16);

    for (let m=1; m<=7; ++m) {
      const isAliased = (gn & (1 << m)) != 0;
      document.getElementById("cbAlias_" + g.toString() + m.toString()).checked = isAliased;
    }
  }
}

function aliasTable_fromHtml(key){
  let val = "00";
  let null_run = 0;

  for(let g=1; g <= 7; ++g) {
    let gn = 0;
    for (let m=1; m<=7; ++m) {
      const isAliased = document.getElementById("cbAlias_" + g.toString() + m.toString()).checked;
      if (isAliased) {
        gn |= (1 << m);
      }
    }
    if (gn == 0) {
      ++null_run;
    } else {
      for (;null_run > 0; --null_run) {
        val = "00"+val;
      }
      const hex =  gn.toString(16);
      val = hex+val;
      if (hex.length < 2)
        val = '0'+val;
    }

  }
  //console.log("at_fromHtml", key, val);
  return val;
}

function aliasTable_fromHtml_toMcu(key){
  const val = aliasTable_fromHtml(key);

  let tfmcu = {"to":"tfmcu", "pair":{"a":key, "mm":val, "c":"store"}};

  var url = base+'/cmd.json';
  http_postRequest(url, tfmcu);

}

function aliasTable_genHtml() {
  let html = "";
  html += '<table id="aliasTable"><tr><th></th>';

  for (let m=1; m <= 7; ++m) {
    html+='<th>m'+m.toString()+'</th>';
  }
  html+= '</tr>';
  for (let g=1; g <= 7; ++g) {
    html += "<tr><th>g"+g.toString()+'</th>';
    for (let m=1; m <=7; ++m) {
      html += '<td><input id="cbAlias_' + g.toString() + m.toString() + '" type="checkbox"></td>';
    }
    html += '</tr>\n';
  }
  html += '</table>';
  return html;
}

function configTr_genHtml(name,value) {
  if (name.endsWith("-enable")) {
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input cb" type="checkbox" id="cfg_'+name+
      '" name="'+name +'"' + (value ? " checked" : "") +'></td>';
  } else if (name === 'verbose') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input" type="number" min="0" max="5" id="cfg_'+name+
      '" name="'+name+'" value="'+value+'"></td>';
  } else if (name === 'network') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><select  class="config-input" id="cfg_'+name+'">'+
      '<option value="wlan">Connect to existing WLAN</option>'+
      '<option value="ap">AP (192.168.4.1, ssid/pw=tronferno)</option>'+
      '<option value="lan">Connect to Ethernet</option>'+ // dev-no-lan-delete-line
      '</select></td>';
  } else if (name === 'lan-phy') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><select  class="config-input" id="cfg_'+name+'">'+
      '<option value="lan8270">LAN8270</option>'+
      '<option value="rtl8201">RTL8201</option>'+
      '<option value="ip101">IP101</option>'+
      '</select></td>';
  } else if (name === 'lan-pwr-gpio') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input" type="number" min="-1" max="36" id="cfg_'+name+
      '" name="'+name+'" value="'+value+'"></td>';
  } else if (name === 'astro-correction') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><select  class="config-input" id="cfg_'+name+'">'+
      '<option value="0">average</option>'+
      '<option value="1">not too late or dark</option>'+
      '<option value="2">not too early or bright</option>'+
      '</select></td>';
  } else {
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input text" type="text" id="cfg_'+name+
      '" name="'+name+'" value="'+value+'"></td>';
  }

}

function usedMembers_fromConfig() {

  let s = ast.tfmcu_config["gm-used"];

  let sa = s ? s.split('').reverse() : [];

  gu = [0];
  for(let g=1; g < 8; ++g) {
    let um = sa[g] ? sa[g] : 0;
    if (um) {
      gu.push(g);
    }
    gmu[g] = um;
  }
}

function usedMembers_updHtml() {
  let val = document.getElementById("cfg_gm-used").value; // HEX string! not a number!

  while(val.length < 8)
    val = "0"+val;

  let g=1;
  gmu = [0];
  gu = [0];
  for(let i=6; i >= 0; --i,++g) {
    let id = "gmu"+(g.toString());
    let mct = parseInt(val[i], 16);
    if (mct)
      gu.push(g);
    gmu.push(mct);
    document.getElementById(id).value = val[i];
  }
}

function usedMembers_fromHtml() {
  let val = "";

  let written = false; // to skip leading zeros
  for(let i=7; i >= 1; --i) {
    let id = "gmu"+(i.toString());
    let mct = document.getElementById(id).value;
    if (mct != '0' || written) {
      val+= mct;
      written = true;
    }
  }
  let x =  document.getElementById("cfg_gm-used").value;
  val += x[x.length-1]; // push back unused lowest nibble

  document.getElementById("cfg_gm-used").value = val;
}

function mcuConfigTable_genHtml(cfg) {
  var html ='<table id="cfg_table_id" class="conf-table">';
  Object.keys(cfg).forEach (function (key, idx) {
    html += '<tr id="cfg_'+key+'_tr">'+configTr_genHtml(key, cfg[key])+'</tr>'+"\n";
  });
  html +='</table>';
  return html;
}

function mcuConfig_updHtml(cfg) {
  Object.keys(cfg).forEach (function (key, idx) {
    let el = document.getElementById('cfg_'+key);

    switch(el.type) {
    case 'checkbox':
      el.checked = cfg[key] != 0;
      break;
    default:
      el.value = cfg[key];
    }
  });
}

function http_postRequest(url = '', data = {}) {
  // Default options are marked with *
  dbLog("post-json: "+JSON.stringify(data));
  return fetch(url, {
    method: "POST", // *GET, POST, PUT, DELETE, etc.
    // mode: "no-cors", // no-cors, cors, *same-origin //dev-delete-line//
    cache: "no-cache", // *default, no-cache, reload, force-cache,
    // only-if-cached
    // credentials: "same-origin", // include, *same-origin, omit
    headers: {
      "Content-Type": "application/json",
    },
    redirect: "follow", // manual, *follow, error
    referrer: "no-referrer", // no-referrer, *client
    body: JSON.stringify(data), // body data type must match "Content-Type"
    // header
  })
    .then(response => {
      if(response.ok) {
        response.json().then(json => {
          ast.http_handleResponses(json);
        });
      }
    });
}

function http_postDocRequest(name) {
  let url = base+'/doc';
  // Default options are marked with *
  return fetch(url, {
    method: "POST", // *GET, POST, PUT, DELETE, etc.
    // mode: "no-cors", // no-cors, cors, *same-origin //dev-delete-line//
    cache: "no-cache", // *default, no-cache, reload, force-cache,
    // only-if-cached
    // credentials: "same-origin", // include, *same-origin, omit
    headers: {
      "Content-Type": "text/plain",
    },
    redirect: "follow", // manual, *follow, error
    referrer: "no-referrer", // no-referrer, *client
    body: name, // body data type must match "Content-Type"
    // header
  })
    .then(response => {
      if(response.ok) {
        response.text().then(text => {
          ast.http_handleDocResponses(name, text);
        });
      }
    });
}


const reload_Progress = {
  ivId: 0,
  ms: 1000,
  count: 10,
  counter: 0,
  divs: ["netota_restart_div", "config_restart_div" ],
};
function req_reloadTick() {
  const rpr = reload_Progress;

  if (++rpr.counter > rpr.count) {
    location.reload();
    clearInterval(rpr.ivId); // may be useless after reload...
  } else {
    document.getElementById("reload_progress_bar").value = rpr.counter;
  }
}
function req_reloadStart() {
  const rpr = reload_Progress;
  let e = null;
  for (let div of rpr.divs) {
    console.log("div", div);
    let e = document.getElementById(div);
    if (e.offsetParent === null)
      continue;

    let html = '<strong>Wait for MCU to restart...</strong><br>';
    html += '<progress id="reload_progress_bar" value="0" max="'+rpr.count.toString()+'">0%</progress>';
    e.innerHTML = html;
    rpr.ivId = setInterval(req_reloadTick, rpr.ms);
    break;
  }

}


function req_mcuRestart() {
  var json = { to:"tfmcu", config: { restart:"1" } };
  var url = base+'/cmd.json';
  http_postRequest(url, json);
  req_reloadStart();
  //setTimeout(function(){ location.reload(); }, 10000);
}


function req_cuasStatus() {
  var json = { to:"tfmcu", config: { cuas:"?" } };
  var url = base+'/cmd.json';
  http_postRequest(url, json);
}

function req_cuasStart() {
  var json = { to:"tfmcu", config: { cu:"auto" } };
  var url = base+'/cmd.json';
  http_postRequest(url, json);
  cuas_State = 0;
  cuas_Interval = window.setInterval(req_cuasStatus, 1000);
}



function mcuConfig_fromHtml_toMcu() {
  let tfmcu = Object.assign({},ast.tfmcu_config);
  var cfg = tfmcu.config;
  var new_cfg = {};
  var has_changed = false;
  usedMembers_fromHtml();
  Object.keys(cfg).forEach (function (key, idx) {
    let new_val = 0;
    let el = document.getElementById('cfg_'+key);
    dbLog("key: "+key);

    switch(el.type) {
    case 'checkbox':
      new_val = el.checked ? 1 : 0;
      break;
    default:
      new_val = el.value;
    }
    let old_val = cfg[key];
    if (new_val != old_val) {
      new_cfg[key] = new_val;
      has_changed = true;
      dbLog(key);
    }
  });

  if (has_changed) {
    new_cfg.all = "?";
    tfmcu.config = new_cfg;

    dbLog(JSON.stringify(tfmcu));
    var url = base+'/cmd.json';
    dbLog("url: "+url);
    http_postRequest(url, tfmcu);
  }
}

function http_postShutterCommand(c=document.getElementById('send-c').value) {
  var tfmcu = {to:"tfmcu"};
  let g = ast.g.toString();
  let m = ast.m.toString();

  var send = {
    g: g,
    m: m,
    c: c,
  };
  tfmcu.send = send;
  dbLog(JSON.stringify(tfmcu));
  var url = base+'/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, tfmcu);
  shuPos_perFetch_start();
}

var netota_intervalID = 0;
var netota_isInProgress = false;
var netota_progressCounter = 0;
function netota_FetchFeedback() {
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    "ota":"?"
  };
  let url = base+'/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, netmcu);
}

const otaName_master = 'github-master';
const otaName_beta = 'github-beta';
function netFirmwareOTA(ota_name) {
  if (netota_isInProgress)
    return;
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    ota: ota_name
  };
  let url = base+'/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, netmcu);
  document.getElementById("netota_progress_div").innerHTML = "<strong>Firmware is updating...<br></strong>" + '<progress id="netota_progress_bar" value="0" max="30">70 %</progress>';
  netota_intervalID = setInterval(netota_FetchFeedback, 1000);
  netota_isInProgress = true;
  document.getElementById("netota_controls").style.display = "none";
}

function getGuIdx() {
  let val = ast.g;

  for(let i=0; i < gu.length; ++i)
    if (gu[i] == val)
      return i;
  return 0;
}

function onGPressed() {
  let val = ast.g;

  gu_idx = getGuIdx(); // FIXME

  ++gu_idx;
  if (gu_idx >= gu.length)
    gu_idx = 0;

  val = gu[gu_idx];

  ast.g = val;
}


function onMPressed() {
  let val = ast.m;

  ++val;
  gu_idx = getGuIdx(); // FIXME
  let g  = gu[gu_idx];

  if (val > gmu[g])
    val = 0;

  ast.m = val;
}

function onPos(pct) {
  console.log("onPos", pct);

  let tfmcu = {to:"tfmcu"};
  tfmcu.send = {
    g: ast.g,
    m: ast.m,
    p: pct,
  };

  let url = base+'/cmd.json';
  http_postRequest(url, tfmcu);
  shuPos_perFetch_start();
}

function req_automatic() {
  let url = base+'/cmd.json';
  let tfmcu = { to:"tfmcu", timer: { }};
  let auto = tfmcu.timer;
  let has_daily = false, has_weekly = false, has_astro = false;

  auto.g = ast.g;
  auto.m = ast.m;

  let f = "i";
  f += document.getElementById('tmci').checked ? "M" : "m";
  f += (has_daily = document.getElementById('tdci').checked) ? "D" : "d";
  f += (has_weekly = document.getElementById('twci').checked) ? "W" : "w";
  f += (has_astro = document.getElementById('taci').checked) ? "A" : "a";
  f += document.getElementById('trci').checked ? "R" : "r";
  f += document.getElementById('tsci').checked ? "S" : "s";
  auto.f = f;

  if (has_weekly)
    auto.weekly = document.getElementById('twti').value;
  if (has_astro)
    auto.astro = document.getElementById('tati').value;
  if (has_daily) {
    let tdu = document.getElementById('tduti').value;
    let tdd = document.getElementById('tddti').value;
    let td = tdu.length !== 5 ? "-" : tdu.substring(0,2) + tdu.substring(3,5);
    td +=  tdd.length !== 5 ? "-" : tdd.substring(0,2) + tdd.substring(3,5);
    auto.daily = td;
  }

  dbLog(JSON.stringify(tfmcu));
  http_postRequest(url, tfmcu);
}

function clearAuto_updHtml() {
  document.getElementById('tfti').value = "";
  document.getElementById('tduti').value = "";
  document.getElementById('tddti').value = "";
  document.getElementById('tati').value = "";
  document.getElementById('twti').value = "";
  document.getElementById('tdci').checked = false;
  document.getElementById('twci').checked = false;
  document.getElementById('taci').checked = false;
  document.getElementById('trci').checked = false;
  document.getElementById('tsci').checked = false;
  document.getElementById('tmci').checked = false;
}

const tabs = [
  { 'text':'Command', 'div_id':['senddiv'], fetch_gm:FETCH_POS},
  { 'text':'Automatic', 'div_id':['senddiv', 'autodiv'], fetch_gm:FETCH_AUTO|FETCH_POS },
  { 'text':'Config', 'div_id':['configdiv'], fetch:FETCH_CONFIG },
  { 'text':'Positions', 'div_id':['senddiv', 'aliasdiv', 'shprefdiv'], fetch:FETCH_ALIASES, fetch_gm:FETCH_POS|FETCH_SHUTTER_PREFS },
  { 'text':'Firmware', 'div_id':['id-fwDiv'], fetch_init:FETCH_VERSION},

];
let div_ids = [];

function navTabs_updHtml(idx) {
  const NONE = "none";
  const SHOW = "";
  const BGC1 = "hsl(220, 60%, 60%)";
  const BGC0 = "#eee";

  if (!(0 <= idx && idx < tabs.length))
    idx = 0;

  for (let i=0; i < tabs.length; ++i) {
    document.getElementById('tabbt'+i.toString()).style.backgroundColor =  (i == idx) ? BGC1 : BGC0;
  }

  for (let div_id of div_ids) {
    document.getElementById(div_id).style.display = tabs[idx].div_id.includes(div_id) ? SHOW : NONE;
  }

  const nt = tabs[idx];

  let fetch = 0;

  if ('fetch_init' in nt) {
    fetch |= nt.fetch_init;
  }
  if ('fetch' in nt) {
    fetch |= nt.fetch;
  }
  if ('fetch_gm' in nt) {
    fetch |= nt.fetch_gm;
    gmc_fetch = nt.fetch_gm;
  } else {
    gmc_fetch = 0;
  }

  if (fetch) {
    ast.http_fetchByMask(fetch);
  }
}

function onNavTab(idx) {
  ast.tabVisibility = idx;
}

function navTabs_genHtml() {
  let html = '';
  for (let i=0; i < tabs.length; ++i) {
    const tab = tabs[i];
    html += '<button class="tablinks" id="tabbt'+i.toString()+'" onClick="onNavTab('+i.toString()+')">'+tab.text+'</button>\n';

    for (let k=0; k < tabs[i].div_id.length; ++k) {
      const div_id = tabs[i].div_id[k];
      if (!div_ids.includes(div_id)) {
        div_ids.push(div_id);
      }
    }
  }
  document.getElementById('tabBar').innerHTML = html;
}


function onContentLoaded() {
  navTabs_genHtml();

  ast = new AppState();
  ast.load();
  http_postDocRequest('cliparm_config');

  document.getElementById("sgb").onclick = () => onGPressed();
  document.getElementById("smb").onclick = () => onMPressed();
  document.getElementById("sub").onclick = () => http_postShutterCommand('up');
  document.getElementById("ssb").onclick = () => http_postShutterCommand('stop');
  document.getElementById("sdb").onclick = () => http_postShutterCommand('down');
  document.getElementById("spb").onclick = () => onPos(document.getElementById("spi").value);
  document.getElementById("spr").onchange = () => onPos(document.getElementById("spr").value);


  document.getElementById("shp_reload").onclick = () => shutterPrefs_updHtml();
  document.getElementById("shp_save").onclick = () => shutterPrefs_fromHtml_toMcu();
  document.getElementById("shp_clockStart").onclick = () => shutterPrefs_stopClock_start();
  document.getElementById("shp_clockStop").onclick = () => shutterPrefs_stopClock_stop();

  document.getElementById("alias_reload").onclick = () => onAliasesReload();
  document.getElementById("alias_save").onclick = () => onAliasesApply();
  document.getElementById("alias_pair").onclick = () => ast.http_fetchByMask(FETCH_ALIASES_START_PAIRING);
  document.getElementById("alias_unpair").onclick = () => ast.http_fetchByMask(FETCH_ALIASES_START_UNPAIRING);


  document.getElementById("arlb").onclick = () => ast.http_fetchByMask(FETCH_AUTO);
  document.getElementById("asvb").onclick = () => {
    // disable button for 5 seconds while data is being sent to shutter
    // motor by RF
    document.getElementById("asvb").disabled = true;
    setTimeout(() => { document.getElementById("asvb").disabled = false; }, 5000);

    req_automatic();
  };
  document.getElementById("csvb").onclick = () => mcuConfig_fromHtml_toMcu();
  document.getElementById("crlb").onclick = () => ast.http_fetchByMask(FETCH_CONFIG);

  document.getElementById("mrtb").onclick = () => req_mcuRestart();

  document.getElementById("netota").onclick = () => netFirmwareOTA(document.getElementById("id-esp32FirmwareURL").value);// dev-distro-delete-line//
  document.getElementById("netota_master").onclick = () => netFirmwareOTA(otaName_master);
  document.getElementById("netota_beta").onclick = () => netFirmwareOTA(otaName_beta);


  document.getElementById("id_cuasb").onclick = () => req_cuasStart();
}

/*
  Local Variables:
  compile-command: "jshint tfmcu_dev.js"
  js-indent-level: 2
  indent-tabs-mode: nil
  End:
*/
