<script context="module">
'use strict';
import * as appState from './app_state.svelte';
import * as httpFetch from './fetch.js';
import * as mcuFirmware from './mcu_firmware.svelte';
import * as navTabs from './nav_tabs.svelte';
  import {G,M} from './store/curr_shutter.js';

export function testing_netota() {
  //document.getElementById("netota").onclick = () => mcuFirmware.netFirmwareOTA(document.getElementById("id-esp32FirmwareURL").value);
}

export function testing_init() {
  navTabs.navTab_addTab({ 'text': 'Tests', 'div_id': ['testsdiv'], }, -1);
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
    G.set(7);
    M.set(1);
    test_randomCmd_interval = setInterval(test_randomCmd, 1000);
  }
}

testing_init();

</script>



 <div id="testsdiv">
 <br><br>Random Periodic Movement Commands:
 <button id="test_rpm_start" type="button" on:click={() => testPressed(true)}>Test Start</button> 
 <button id="test_rpm_stop" type="button" on:click={() => testPressed(false)}>Test Stop</button>
 </div>
