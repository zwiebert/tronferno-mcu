<script context="module">
'use strict';
import * as appState from './app_state.js';
import * as httpFetch from './fetch.js';
import * as mcuFirmware from './mcu_firmware.svelte';
import * as navTabs from './nav_tabs.svelte';

export function testing_netota() {
  document.getElementById("netota").onclick = () => mcuFirmware.netFirmwareOTA(document.getElementById("id-esp32FirmwareURL").value);
}

export function testing_init() {
  navTabs.tabs.push({ 'text': 'Tests', 'div_id': ['testsdiv'], });
}

export function testing_cbHtml() {
  document.getElementById("test_rpm_start").onclick = () => testPressed(true);
  document.getElementById("test_rpm_stop").onclick = () => testPressed(false);
}

export function testing_genHtml() {
  let html = "";
  html += test_random_periodic_movement_genHtml();
  return html;
}

function test_random_periodic_movement_genHtml() {
  let html = '<br><br>Random Periodic Movement Commands: <button id="test_rpm_start" type="button">Test Start</button> <button id="test_rpm_stop" type="button">Test Stop</button>';

  return html;
}

const test_cmds = ['up', 'down', 'stop'];
function get_randomCmd() {
  const idx = Math.floor(Math.random() * (test_cmds.length - 0.01));
  return test_cmds[idx];
}

function test_randomCmd() {
  const cmd = get_randomCmd();
  console.log(cmd);
  httpFetch.http_postShutterCommand(cmd);
}

let test_randomCmd_interval = 0;
function testPressed(enable) {

  if (test_randomCmd_interval) {
    clearInterval(test_randomCmd_interval);
    test_randomCmd_interval = 0;
  }

  if (enable) {
    appState.ast.g = 7;
    appState.ast.m = 1;
    test_randomCmd_interval = setInterval(test_randomCmd, 1000);
  }
}
</script>

 <div id="testsdiv">
 {@html testing_genHtml()} 
 </div>
