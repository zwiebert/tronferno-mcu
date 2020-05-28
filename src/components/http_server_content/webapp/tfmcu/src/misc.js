'use strict';

import * as appState from './app_state.js';
import * as shutterPrefs from './shutter_prefs.svelte';
import * as shutterBasic from './shutter_basic.svelte';
import * as shutterAlias from './shutter_alias.svelte';
import * as shutterAuto from './shutter_auto.svelte';
import * as mcuFirmware from './mcu_firmware.svelte';
import * as httpFetch from './fetch.js';
import * as cuas from './cuas.js';
import * as mcuConfig from './mcu_config.svelte';
import * as navTabs from './nav_tabs.svelte';
import * as testing from './testing.svelte';

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
  httpFetch.http_postRequest(url, json);
  req_reloadStart();
  //setTimeout(function(){ location.reload(); }, 10000);
}

export function app_cbHtml() {
  navTabs.navTabs_cbHtml();
  shutterBasic.basic_cbHtml();
  shutterPrefs.shp_cbHtml();
  shutterAlias.alias_cbHtml();
  shutterAuto.auto_cbHtml();
  mcuFirmware.netota_cbHtml();
  mcuConfig.mcuConfig_cbHtml();
  testing.testing_cbHtml();

  document.getElementById("crlb").onclick = () => httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
  document.getElementById("mrtb").onclick = () => req_mcuRestart();
  document.getElementById("id_cuasb").onclick = () => cuas.req_cuasStart();
}

export function onContentLoaded() {
  appState.init();
  httpFetch.http_postDocRequest('cliparm_config');
}
