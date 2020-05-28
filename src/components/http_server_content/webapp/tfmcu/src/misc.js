'use strict';


import * as as from './app_state.js';
import * as shutterPrefs from './shutter_prefs.js';
import * as shutterName from './shutter_name.js';
import * as ota from './netota.js';
import * as a_http from './fetch.js';
import * as cuas from './cuas.js';
import * as mcc from './mcu_config.js';



export function dbLog(msg) { console.log(msg); }


export function aliasControllers_updHtml() {
  if (document.getElementById("divPairAll").innerHTML.length < 20) {
    document.getElementById("divPairAll").innerHTML = aliasTable_genHtml();
  }

  const pad = as.ast.aliases;
  const pas = document.getElementById("aliases");
  const pas_sel = pas.selectedIndex;
  for (const key in pad) {
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
  const pad = as.ast.aliases;
  const pas = document.getElementById("aliases");
  const pas_sel = pas.selectedIndex;

  for (let opt of pas.options) {
    const key = opt.text;
    aliasTable_fromHtml(key);
  }
}

function onAliasesChanged() {
  const pas = document.getElementById("aliases");
  if (pas.selectedIndex >= 0) {
    const key = pas.options[pas.selectedIndex].text;
    document.getElementById("paired").value = key;
    aliasTable_updHtml(key);
  }
}

function onPairedChanged() {
  let pas = document.getElementById("paired");
  if (pas.selectedIndex >= 0) {
    const key = pas.options[pas.selectedIndex].text;
    document.getElementById("aliases").value = key;
    aliasTable_updHtml(key);
  }
}


function onAliasesApply() {
  const pas = document.getElementById("aliases");
  if (pas.selectedIndex >= 0) {
    const key = pas.options[pas.selectedIndex].text;
    aliasTable_fromHtml_toMcu(key);
  }
}
function onAliasesReload() {
  a_http.http_fetchByMask(a_http.FETCH_ALIASES);
}

function alias_isKeyPairedToM(key, g, m) {
  const val = as.ast.aliases[key];

  let chunks = [];

  for (let i = 0, charsLength = val.length; i < charsLength; i += 2) {
    chunks.unshift(val.substring(i, i + 2));
  }

  const g_max = chunks.length - 1;

  if (g > g_max)
    return false;

  const b = parseInt(chunks[g], 16);

  return (b & (1 << m)) != 0;

}

export function aliasPaired_updHtml() {
  const g = as.ast.g;
  const m = as.ast.m;
  const pas = document.getElementById("paired");

  for (let i = pas.options.length - 1; i >= 0; --i)
  pas.remove(i);
  for (let key in as.ast.aliases) {
    if (!alias_isKeyPairedToM(key, g, m))
      continue;

    let option = document.createElement("option");
    option.text = key;
    pas.add(option);
  }
}
function aliasTable_updHtml(key) {
  const val = as.ast.aliases[key];

  let chunks = [];

  for (let i = 0, charsLength = val.length; i < charsLength; i += 2) {
    chunks.unshift(val.substring(i, i + 2));
  }

  const g_max = chunks.length - 1;

  for (let g = 1; g <= 7; ++g) {

    const gn = (g > g_max) ? 0 : parseInt(chunks[g], 16);

    for (let m = 1; m <= 7; ++m) {
      const isAliased = (gn & (1 << m)) != 0;
      document.getElementById("cbAlias_" + g.toString() + m.toString()).checked = isAliased;
    }
  }
}

function aliasTable_fromHtml(key) {
  let val = "00";
  let null_run = 0;

  for (let g = 1; g <= 7; ++g) {
    let gn = 0;
    for (let m = 1; m <= 7; ++m) {
      const isAliased = document.getElementById("cbAlias_" + g.toString() + m.toString()).checked;
      if (isAliased) {
        gn |= (1 << m);
      }
    }
    if (gn == 0) {
      ++null_run;
    } else {
      for (; null_run > 0; --null_run) {
        val = "00" + val;
      }
      const hex = gn.toString(16);
      val = hex + val;
      if (hex.length < 2)
        val = '0' + val;
    }

  }
  //console.log("at_fromHtml", key, val);
  return val;
}

function aliasTable_fromHtml_toMcu(key) {
  const val = aliasTable_fromHtml(key);

  let tfmcu = { "to": "tfmcu", "pair": { "a": key, "mm": val, "c": "store" } };

  var url = '/cmd.json';
  a_http.http_postRequest(url, tfmcu);

}

function aliasTable_genHtml() {
  let html = "";
  html += '<table id="aliasTable"><tr><th></th>';

  for (let m = 1; m <= 7; ++m) {
    html += '<th>m' + m.toString() + '</th>';
  }
  html += '</tr>';
  for (let g = 1; g <= 7; ++g) {
    html += "<tr><th>g" + g.toString() + '</th>';
    for (let m = 1; m <= 7; ++m) {
      html += '<td><input id="cbAlias_' + g.toString() + m.toString() + '" type="checkbox"></td>';
    }
    html += '</tr>\n';
  }
  html += '</table>';
  return html;
}


const reload_Progress = {
  ivId: 0,
  ms: 1000,
  count: 12,
  counter: 0,
  divs: ["netota_restart_div", "config_restart_div"],
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
    let e = document.getElementById(div);
    if (e.offsetParent === null)
      continue;

    let html = '<strong>Wait for MCU to restart...</strong><br>';
    html += '<progress id="reload_progress_bar" value="0" max="' + rpr.count.toString() + '">0%</progress>';
    e.innerHTML = html;
    rpr.ivId = setInterval(req_reloadTick, rpr.ms);
    break;
  }

}


export function req_mcuRestart() {
  var json = { to: "tfmcu", config: { restart: "1" } };
  var url = '/cmd.json';
  a_http.http_postRequest(url, json);
  req_reloadStart();
  //setTimeout(function(){ location.reload(); }, 10000);
}





// ------------ cmd div --------------
function onGPressed() {
  as.ast.g_next();
}

function onMPressed() {
  as.ast.m_next();
}

function onPos(pct) {
  let tfmcu = { to: "tfmcu" };
  tfmcu.send = {
    g: as.ast.g,
    m: as.ast.m,
    p: pct,
  };

  let url = '/cmd.json';
  a_http.http_postRequest(url, tfmcu);
}

// -------------  auto div -------------------------------
function req_automatic() {
  let url = '/cmd.json';
  let tfmcu = { to: "tfmcu", timer: {} };
  let auto = tfmcu.timer;
  let has_daily = false, has_weekly = false, has_astro = false;

  auto.g = as.ast.g;
  auto.m = as.ast.m;

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
    let td = tdu.length !== 5 ? "-" : tdu.substring(0, 2) + tdu.substring(3, 5);
    td += tdd.length !== 5 ? "-" : tdd.substring(0, 2) + tdd.substring(3, 5);
    auto.daily = td;
  }

  dbLog(JSON.stringify(tfmcu));
  a_http.http_postRequest(url, tfmcu);
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

//--------------- nav tabs ------------------
export let tabs = [
  { 'text': 'Command', 'div_id': ['senddiv'], fetch_gm: (a_http.FETCH_POS | a_http.FETCH_SHUTTER_NAME) },
  { 'text': 'Automatic', 'div_id': ['senddiv', 'autodiv'], fetch_gm: (a_http.FETCH_AUTO | a_http.FETCH_POS | a_http.FETCH_SHUTTER_NAME) },
  { 'text':'Config', 'div_id':['configdiv'], fetch:a_http.FETCH_CONFIG },
  { 'text':'Positions', 'div_id':['senddiv', 'aliasdiv', 'shprefdiv'], fetch:a_http.FETCH_ALIASES, fetch_gm:a_http.FETCH_POS|a_http.FETCH_SHUTTER_PREFS|a_http.FETCH_SHUTTER_NAME },
  { 'text':'Firmware', 'div_id':['id-fwDiv'], fetch_init:(a_http.FETCH_VERSION|a_http.FETCH_GIT_TAGS)},
];
let div_ids = [];

export function navTabs_updHtml(idx) {
  const NONE = "none";
  const SHOW = "";
  const BGC1 = "hsl(220, 60%, 60%)";
  const BGC0 = "#eee";

  if (!(0 <= idx && idx < tabs.length))
    idx = 0;

  for (let i = 0; i < tabs.length; ++i) {
    document.getElementById('tabbt' + i.toString()).style.backgroundColor = (i == idx) ? BGC1 : BGC0;
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
    as.ast.gmc_fetch = nt.fetch_gm;
  } else {
    as.ast.gmc_fetch = 0;
  }

  if (fetch) {
    a_http.http_fetchByMask(fetch);
  }
}

function onNavTab(idx) {
  as.ast.tabVisibility = idx;
}

function navTabs_genHtml() {
  let html = '';
  for (let i = 0; i < tabs.length; ++i) {
    const tab = tabs[i];
    html += '<button class="tablinks" id="tabbt'+i.toString()+'">'+tab.text+'</button>\n';

    for (let k = 0; k < tabs[i].div_id.length; ++k) {
      const div_id = tabs[i].div_id[k];
      if (!div_ids.includes(div_id)) {
        div_ids.push(div_id);
      }
    }
  }
  document.getElementById('tabBar').innerHTML = html;
  for (let i = 0; i < tabs.length; ++i) {
    document.getElementById('tabbt' +  i.toString()).onclick = () => onNavTab(i);
  }
}

// ----------------------------------------
export function onContentLoaded() {
  navTabs_genHtml();

  as.init();

  a_http.http_postDocRequest('cliparm_config');

  document.getElementById("smn").onchange = () => shutterName.shn_fromHtml_toMcu();

  document.getElementById("sgb").onclick = () => onGPressed();
  document.getElementById("smb").onclick = () => onMPressed();
  document.getElementById("sub").onclick = () => a_http.http_postShutterCommand('up');
  document.getElementById("ssb").onclick = () => a_http.http_postShutterCommand('stop');
  document.getElementById("sdb").onclick = () => a_http.http_postShutterCommand('down');
  document.getElementById("sspb").onclick = () => a_http.http_postShutterCommand('sun-down');
  document.getElementById("spb").onclick = () => onPos(document.getElementById("spi").value);
  document.getElementById("spr").onchange = () => onPos(document.getElementById("spr").value);

  shutterPrefs.shp_cbHtml();

  document.getElementById("alias_reload").onclick = () => onAliasesReload();
  document.getElementById("alias_save").onclick = () => onAliasesApply();
  document.getElementById("alias_pair").onclick = () => a_http.http_fetchByMask(a_http.FETCH_ALIASES_START_PAIRING);
  document.getElementById("alias_unpair").onclick = () => a_http.http_fetchByMask(a_http.FETCH_ALIASES_START_UNPAIRING);


  document.getElementById("arlb").onclick = () => a_http.http_fetchByMask(a_http.FETCH_AUTO);
  document.getElementById("asvb").onclick = () => {
    // disable button for 5 seconds while data is being sent to shutter
    // motor by RF
    document.getElementById("asvb").disabled = true;
    setTimeout(() => { document.getElementById("asvb").disabled = false; }, 5000);

    req_automatic();
  };
  document.getElementById("csvb").onclick = () => mcc.mcuConfig_fromHtml_toMcu();
  document.getElementById("crlb").onclick = () => a_http.http_fetchByMask(a_http.FETCH_CONFIG);

  document.getElementById("mrtb").onclick = () => req_mcuRestart();

  ota.netota_cbHtml();



  document.getElementById("id_cuasb").onclick = () => cuas.req_cuasStart();

  document.getElementById("aliases").onchange = () => onAliasesChanged();
  document.getElementById("paired").onclick = () => onPairedChanged();
  document.getElementById("paired").onchange = () => onPairedChanged();

}

/*
  Local Variables:
  compile-command: "jshint tfmcu_dev.js"
  js-indent-level: 2
  indent-tabs-mode: nil
  End:
*/
