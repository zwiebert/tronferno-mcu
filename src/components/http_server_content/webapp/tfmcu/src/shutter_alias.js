'use strict';
import * as aps from './app_state';
import * as a_http from './fetch.js';

export function alias_cbHtml() {
  document.getElementById("alias_reload").onclick = () => onAliasesReload();
  document.getElementById("alias_save").onclick = () => onAliasesApply();
  document.getElementById("alias_pair").onclick = () => a_http.http_fetchByMask(a_http.FETCH_ALIASES_START_PAIRING);
  document.getElementById("alias_unpair").onclick = () => a_http.http_fetchByMask(a_http.FETCH_ALIASES_START_UNPAIRING);
  document.getElementById("aliases").onchange = () => onAliasesChanged();
  document.getElementById("paired").onclick = () => onPairedChanged();
  document.getElementById("paired").onchange = () => onPairedChanged();
}

export function aliasControllers_updHtml() {
  if (document.getElementById("divPairAll").innerHTML.length < 20) {
    document.getElementById("divPairAll").innerHTML = aliasTable_genHtml();
  }

  const pad = aps.ast.aliases;
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
  const pad = aps.ast.aliases;
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
  const val = aps.ast.aliases[key];

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
  const g = aps.ast.g;
  const m = aps.ast.m;
  const pas = document.getElementById("paired");

  for (let i = pas.options.length - 1; i >= 0; --i)
  pas.remove(i);
  for (let key in aps.ast.aliases) {
    if (!alias_isKeyPairedToM(key, g, m))
      continue;

    let option = document.createElement("option");
    option.text = key;
    pas.add(option);
  }
}
function aliasTable_updHtml(key) {
  const val = aps.ast.aliases[key];

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
