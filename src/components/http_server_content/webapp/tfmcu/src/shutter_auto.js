'use strict';
import { dbLog } from './misc.js';
import * as aps from './app_state';
import * as a_http from './fetch.js';

export function auto_cbHtml() {
  document.getElementById("arlb").onclick = () => a_http.http_fetchByMask(a_http.FETCH_AUTO);
  document.getElementById("asvb").onclick = () => {
    // disable button for 5 seconds while data is being sent to shutter
    // motor by RF
    document.getElementById("asvb").disabled = true;
    setTimeout(() => { document.getElementById("asvb").disabled = false; }, 5000);

    req_automatic();
  };
}

// -------------  auto div -------------------------------
function req_automatic() {
  let url = '/cmd.json';
  let tfmcu = { to: "tfmcu", timer: {} };
  let auto = tfmcu.timer;
  let has_daily = false, has_weekly = false, has_astro = false;

  auto.g = aps.ast.g;
  auto.m = aps.ast.m;

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