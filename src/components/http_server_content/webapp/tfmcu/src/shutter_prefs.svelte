<script>
'use strict';
import * as httpFetch from './fetch.js';
import {G,M, GM,PrefMvut,PrefMvdt,PrefMvspdt} from './store/curr_shutter.js';
import { onMount, onDestroy } from 'svelte';

export const UP = 0;
export const DOWN = 1;
export const SUN_DOWN = 2;

let on_destroy = [];
onMount(() => {
    on_destroy.push(GM.subscribe(() => shp_updHtml()));
  });
onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
});

let mvut = 0;
let mvdt = 0;
let mvspdt = 0;

function shp_updHtml() {
   mvut = $PrefMvut / 10.0;
   mvdt = $PrefMvdt / 10.0;
   mvspdt = $PrefMvspdt;
}

function hClick_Reload() {
  shp_updHtml();
}

function hClick_Save() {
  shp_fromHtml_toMcu();
}

function hClick_Up() {
  shp_stopClock_do(UP);
}

function hClick_Down() {
  shp_stopClock_do(DOWN);
}

function hClick_Sun() {
  shp_stopClock_do(SUN_DOWN);
}

function shp_fromHtml_toMcu() {
  const mvut = document.getElementById("shpMvut");
  const mvdt = document.getElementById("shpMvdt");
  const mvspdt = document.getElementById("shpSpMvdt");

  let tfmcu = { "to": "tfmcu", "shpref": { "g": $G, "m": $M, "c": "store" } };
  let pref = tfmcu.shpref;

  pref.mvut = Math.floor((parseFloat(mvut.value) * 10)).toString();
  pref.mvdt = Math.floor((parseFloat(mvdt.value) * 10)).toString();
  pref.mvspdt = Math.floor((parseFloat(mvspdt.value) * 10)).toString();

  let url = '/cmd.json';
  httpFetch.http_postRequest(url, tfmcu);
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
  let elem = document.getElementById(spsc.direction === UP ? "shpMvut" : spsc.direction === DOWN ? "shpMvdt" : spsc.direction === SUN_DOWN ? "shpSpMvdt" : -1);

  spsc.val += (spsc.ms / 100);
  
  if (spsc.direction === UP) {
    mvut = spsc.val;
  } else if (spsc.direction === DOWN) {
    mvdt = spsc.val;
  } else {
    mvspdt = spsc.val;
  }
}

function shp_stopClock_do(direction) {
  let spsc = stopClock;

  if (spsc.ivId) {
    shp_stopClock_stop();
    return;
  }

  if (direction === UP) {
    spsc.direction = UP;
    httpFetch.http_postShutterCommand('up');
  } else if (direction === DOWN) {
    spsc.direction = DOWN;
    httpFetch.http_postShutterCommand('down');
  } else if (direction === SUN_DOWN) {
    spsc.direction = SUN_DOWN;
    httpFetch.http_postShutterCommand('sun-down');
  } else {
    return;
  }

  if (!spsc.ivId) {
    spsc.val = 0;
    spsc.ivId = setInterval(shp_stopClock_tick, spsc.ms);
  }
}

function shp_stopClock_stop() {
  let spsc = stopClock;
  if (spsc.ivId) {
    clearInterval(spsc.ivId);
    spsc.ivId = 0;
  }
}
</script>

<div id="shprefdiv">
  <hr>
  <h4>Moving Durations</h4>
  <p class="help_text">To measure the duration of up/down movements, press Start/Stop to start both movement and clock. After the movement stops by itself, press again to stop the clock.</p>
  <br>
  <table class="dur_table">
    <tr><th>Direction</th><th>Duration (secs)</th><th>Stopclock</th></tr>
    <tr><td>up to 100%</td><td><input id="shpMvut" type="number" min="0" bind:value="{mvut}" step="0.1"></td><td><button id="shp_MvutButton" on:click={hClick_Up}>Start/Stop</button></td></tr>
    <tr><td>down to 0%</td><td><input id="shpMvdt" type="number" min="0" bind:value="{mvdt}" step="0.1"></td><td><button id="shp_MvdtButton" on:click={hClick_Down}>Start/Stop</button></td></tr>
    <tr><td>down to sun position</td><td><input id="shpSpMvdt" type="number" min="0" bind:value="{mvspdt}" step="0.1"></td><td><button id="shp_SpMvdtButton" on:click={hClick_Sun}>Start/Stop</button></td></tr>
  </table>
  <button id="shp_reload" type="button" on:click={hClick_Reload}>Reload</button>
  <button id="shp_save" type="button" on:click={hClick_Save}>Save</button>
</div>