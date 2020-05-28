'use strict';
import * as as from './app_state';
import * as a_http from './fetch.js';

export const UP = 0;
export const DOWN = 1;
export const SUN_DOWN = 2;

export function shp_cbHtml() {
  document.getElementById("shp_reload").onclick = () => shp_updHtml();
  document.getElementById("shp_save").onclick = () => shp_fromHtml_toMcu();
  document.getElementById("shp_MvutButton").onclick = () => shp_stopClock_do(UP);
  document.getElementById("shp_MvdtButton").onclick = () => shp_stopClock_do(DOWN);
  document.getElementById("shp_SpMvdtButton").onclick = () => shp_stopClock_do(SUN_DOWN);
}

export function shp_updHtml() {
  const key = "shs" + as.ast.g.toString() + as.ast.m.toString();
  let pref = as.ast.shutterPrefs[key];
  const mvut = document.getElementById("shpMvut");
  const mvdt = document.getElementById("shpMvdt");
  const mvspdt = document.getElementById("shpSpMvdt");

  if (!pref) {
    pref = { mvut: 0, mvdt: 0, mvspdt: 0 };
  }

  if (pref) {
    mvut.value = (parseFloat(pref.mvut) / 10.0).toString();
    mvdt.value = (parseFloat(pref.mvdt) / 10.0).toString();
    mvspdt.value = (parseFloat(pref.mvspdt) / 10.0).toString();
  }
}

export function shp_fromHtml_toMcu() {
  const mvut = document.getElementById("shpMvut");
  const mvdt = document.getElementById("shpMvdt");
  const mvspdt = document.getElementById("shpSpMvdt");

  let tfmcu = { "to": "tfmcu", "shpref": { "g": as.ast.g, "m": as.ast.m, "c": "store" } };
  let pref = tfmcu.shpref;

  pref.mvut = Math.floor((parseFloat(mvut.value) * 10)).toString();
  pref.mvdt = Math.floor((parseFloat(mvdt.value) * 10)).toString();
  pref.mvspdt = Math.floor((parseFloat(mvspdt.value) * 10)).toString();

  var url = '/cmd.json';
  a_http.http_postRequest(url, tfmcu);
}


let stopClock = {
  ms: 100,
  direction: 0,
  ivId: 0,
  text_elem: null,
  val: 0,
};

function shp_stopClock_tick() {
  let spsc = stopClock;
  let elem = document.getElementById(spsc.direction == UP ? "shpMvut" : spsc.direction == DOWN ? "shpMvdt" : spsc.direction == SUN_DOWN ? "shpSpMvdt" : -1);

  spsc.val += (spsc.ms / 100);
  elem.value = (spsc.val / 10.0).toString();
}

export function shp_stopClock_do(direction) {
  let spsc = stopClock;

  if (spsc.ivId) {
    shp_stopClock_stop();
    return;
  }

  if (direction === UP) {
    spsc.direction = UP;
    a_http.http_postShutterCommand('up');
  } else if (direction === DOWN) {
    spsc.direction = DOWN;
    a_http.http_postShutterCommand('down');
  } else if (direction === SUN_DOWN) {
    spsc.direction = SUN_DOWN;
    a_http.http_postShutterCommand('sun-down');
  } else {
    return;
  }

  if (!spsc.ivId) {
    spsc.val = 0;
    spsc.ivId = setInterval(shp_stopClock_tick, spsc.ms);
  }
}

export function shp_stopClock_stop() {
  let spsc = stopClock;
  if (spsc.ivId) {
    clearInterval(spsc.ivId);
    spsc.ivId = 0;
  }
}
