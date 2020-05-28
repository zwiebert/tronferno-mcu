'use strict';
import * as a_http from './fetch.js';
import * as as from './app_state.js';

let cuas_Interval;
let cuas_State = 0;

export function req_cuasStatus() {
  var json = { to: "tfmcu", config: { cuas: "?" } };
  var url = '/cmd.json';
  a_http.http_postRequest(url, json);
}

export function req_cuasStart() {
  var json = { to: "tfmcu", config: { cu: "auto" } };
  var url = '/cmd.json';
  a_http.http_postRequest(url, json);
  cuas_State = 0;
  cuas_Interval = window.setInterval(req_cuasStatus, 1000);
}

export function cuas_handle_cuasState(config) {
  if (config.cuas == cuas_State)
    return;

  let s = "";
  switch (config.cuas) {
    case 0: s = ""; break;
    case 1: s = '<strong style="animation: blink .75s linear 4;"> ...Scanning...(press STOP on Central Unit) </strong>'; break;
    case 2: s = '<strong style="background-color:red;">Time-Out! (no STOP-command received)</strong>'; break;
    case 3: s = '<strong style="background-color:green;">Success (cu-id has been saved)</strong>';
      a_http.http_fetchByMask(a_http.FETCH_CONFIG);
      break;
  }

  if (config.cuas > 1) {
    window.clearInterval(cuas_Interval);
  }
  document.getElementById("id_cuasStatus").innerHTML = s;
  config.cuas = cuas_State;
}