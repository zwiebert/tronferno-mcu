'use strict';


import * as appState from './app_state.js';
import * as shutterPrefs from './shutter_prefs.js';
import * as shutterName from './shutter_name.js';
import * as shutterAlias from './shutter_alias.js';
import * as shutterAuto from './shutter_auto.js';
import * as mcuFirmware from './netota.js';
import * as httpFetch from './fetch.js';
import * as cuas from './cuas.js';
import * as mcuConfig from './mcu_config.js';
import * as navTabs from './nav_tabs.js';





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





// ------------ cmd div --------------
function onGPressed() {
  appState.ast.g_next();
}

function onMPressed() {
  appState.ast.m_next();
}

function onPos(pct) {
  let tfmcu = { to: "tfmcu" };
  tfmcu.send = {
    g: appState.ast.g,
    m: appState.ast.m,
    p: pct,
  };

  let url = '/cmd.json';
  httpFetch.http_postRequest(url, tfmcu);
}



// ----------------------------------------
export function onContentLoaded() {
  navTabs.navTabs_genHtml();

  appState.init();

  httpFetch.http_postDocRequest('cliparm_config');

  shutterPrefs.shp_cbHtml();
  shutterAlias.alias_cbHtml();
  shutterAuto.auto_cbHtml();
  mcuFirmware.netota_cbHtml();
  mcuConfig.mcuConfig_cbHtml();

  document.getElementById("smn").onchange = () => shutterName.shn_fromHtml_toMcu();

  document.getElementById("sgb").onclick = () => onGPressed();
  document.getElementById("smb").onclick = () => onMPressed();
  document.getElementById("sub").onclick = () => httpFetch.http_postShutterCommand('up');
  document.getElementById("ssb").onclick = () => httpFetch.http_postShutterCommand('stop');
  document.getElementById("sdb").onclick = () => httpFetch.http_postShutterCommand('down');
  document.getElementById("sspb").onclick = () => httpFetch.http_postShutterCommand('sun-down');
  document.getElementById("spb").onclick = () => onPos(document.getElementById("spi").value);
  document.getElementById("spr").onchange = () => onPos(document.getElementById("spr").value);




  

  document.getElementById("crlb").onclick = () => httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);

  document.getElementById("mrtb").onclick = () => req_mcuRestart();

  document.getElementById("id_cuasb").onclick = () => cuas.req_cuasStart();

}

/*
  Local Variables:
  compile-command: "jshint tfmcu_dev.js"
  js-indent-level: 2
  indent-tabs-mode: nil
  End:
*/
