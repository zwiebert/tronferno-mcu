<script>
  'use strict';
  import {McuConfig, McuConfigKeys, Gmu} from './store/mcu_config.js';
  import * as appDebug from './app_debug.js';
  import * as httpFetch from './fetch.js';
  import * as cuas from './cuas.js';
  import * as misc from './misc.js';
  import { onMount,onDestroy } from 'svelte';
  
  let on_destroy = [];
onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
  });
onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
});


  $: mcuConfigKeys = $McuConfigKeys;
  $: mcuConfig = $McuConfig;
  $: gmu = $Gmu;

  export function reload_config() {
  // eslint-disable-next-line no-unused-vars
  Object.keys($McuConfig).forEach(function(key, idx) {
    let el = document.getElementById('cfg_' + key);

    switch (el.type) {
      case 'checkbox':
        el.checked = $McuConfig[key] !== 0;
        break;
      default:
        el.value = $McuConfig[key];
    }
  });

       for(let i=1; i <= 7;++i) {
        document.getElementById('gmu'+i.toString()).value = $Gmu[i];
     }
  }


  function hClick_Reload() {

    reload_config();
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
  }

  function hClick_Save() {
    usedMembers_fromHtml_toHtml();
    mcuConfig_fromHtml_toMcu();
  }

  function hClick_RestartMcu() {
    misc.req_mcuRestart();
  }

  function hClick_ScanCU() {
    cuas.req_cuasStart();
  }

 function mcuConfig_fromHtml_toMcu() {
  const cfg = $McuConfig;

  let new_cfg = {};
  let has_changed = false;
  usedMembers_fromHtml_toHtml();
  // eslint-disable-next-line no-unused-vars
  Object.keys(cfg).forEach(function(key, idx) {
    let new_val = 0;
    let el = document.getElementById('cfg_' + key);
    appDebug.dbLog("key: " + key);

    switch (el.type) {
      case 'checkbox':
        new_val = el.checked ? 1 : 0;
        break;
      default:
        new_val = el.value;
    }
    let old_val = cfg[key];
    if (new_val !== old_val) {
      new_cfg[key] = new_val;
      has_changed = true;
      appDebug.dbLog(key);
    }
  });

  if (has_changed) {
    new_cfg.all = "?";
    let url = '/cmd.json';
    httpFetch.http_postRequest(url, { config: new_cfg });
  }
}

function usedMembers_fromHtml_toHtml() {
  let val = "";

  let written = false; // to skip leading zeros
  for (let i = 7; i >= 1; --i) {
    let id = "gmu" + (i.toString());
    let mct = document.getElementById(id).value;
    if (mct !== '0' || written) {
      val += mct;
      written = true;
    }
  }
  let x = document.getElementById("cfg_gm-used").value;
  val += x[x.length - 1]; // push back unused lowest nibble

  document.getElementById("cfg_gm-used").value = val;
}
</script>

<style type="text/scss">
.conf-table label { padding: 2px; width: 50vw; margin-right: 6px;}
.conf-table .text { width: 100%; }

</style>

<div id="configdiv">
  <div  class="config" id="config-div">
  <table id="cfg_table_id" class="conf-table">
  {#each mcuConfigKeys as name}
  <tr>
  {#if (name.endsWith("-enable"))}
        <td><label class="config-label">{name}</label></td>
        <td><input class="config-input cb" type="checkbox" id="cfg_{name}" name="{name}" checked={mcuConfig[name]}>
        </td>
  {:else if (name === 'rf-rx-pin' || name === 'set-button-pin')}
        <td><label class="config-label">{name}</label></td>
        <td><input class="config-input" type="number" min="-1" max="39" id="cfg_{name}" name="{name}" value="{mcuConfig[name]}"></td>
  {:else if (name === 'rf-tx-pin')}
        <td><label class="config-label">{name}</label></td><td><input class="config-input" type="number" min="-1" max="33" id="cfg_{name}" name="{name}" value="{mcuConfig[name]}"></td>
  {:else if (name === 'verbose')}
        <td><label class="config-label">{name}</label></td><td><input class="config-input" type="number" min="0" max="5" id="cfg_{name}" name="{name}" value="{mcuConfig[name]}"></td>
  {:else if (name === 'network')}
      <td><label class="config-label">{name}</label></td><td><select  class="config-input" id="cfg_{name}">
      <option value="wlan">Connect to existing WLAN</option>
      <option value="ap">AP (192.168.4.1, ssid/pw=tronferno)</option>
      <option value="lan">Connect to Ethernet</option> <!-- dev-no-lan-delete-line --> 
      <option value="none">No Network</option>
      </select></td>
  {:else if (name === 'lan-phy')}
        <td><label class="config-label">{name}</label></td><td><select  class="config-input" id="cfg_{name}">
      <option value="lan8270">LAN8270</option>
      <option value="rtl8201">RTL8201</option>
      <option value="ip101">IP101</option>
      </select></td>
  {:else if (name === 'lan-pwr-gpio')}
       <td><label class="config-label">{name}</label></td><td><input class="config-input" type="number" min="-1" max="36" id="cfg_{name}" name="{name}" value="{mcuConfig[name]}"></td>
  {:else if (name === 'astro-correction')}
       <td><label class="config-label">{name}</label></td><td><select  class="config-input" id="cfg_{name}">
      <option value="0">average</option>
      <option value="1">not too late or dark</option>
      <option value="2">not too early or bright</option>
      </select></td>
  {:else if (name.startsWith('gpio'))}
       <td><label class="config-label">{name}</label></td><td><select  class="config-input" id="cfg_{name}">
      <option value="i">Input (Pull.FLoating)</option>
      <option value="ih">Input (Pull.Up)</option>
      <option value="il">Input (Pull.Down)</option>
      <option value="O">Output</option>
      <option value="Ol">Output (Level.Low)</option>
      <option value="Oh">Output (Level.High)</option>
      <option value="o">Output (OpenDrain)</option>
      <option value="ol">Output (OpenDrain + Level.Low)</option>
      <option value="oh">Output (OpenDrain + Level.High)</option>
      <option value="Q">Input/Output</option>
      <option value="Ql">Input/Output (Level.Low)</option>
      <option value="Qh">Input/Output (Level.High)</option>
      <option value="q">Input/Output (OpenDrain)</option>
      <option value="ql">Input/Output (OpenDrain + Level.Low)</option>
      <option value="qh">Input/Output (OpenDrain + Level.High)</option>
      </select></td>
  {:else if name !== 'gm-used'}
      <td><label class="config-label">{name}</label></td><td><input class="config-input text" type="text" id="cfg_{name}" name="{name}" value="{mcuConfig[name]}"></td>
  {/if}

  </tr>
  {/each}

  </table>
    
  </div>

  <input type="text" id="cfg_gm-used" value="{mcuConfig['gm-used']}" style="display:none;"/>

  <table id="gmu-table">
    <tr>
      <td><label class="config-label">Groups</label></td>
      {#each {length:7} as _, i}
        <td><label>{i+1}</label></td>
      {/each}
    </tr>
    <tr>
      <td><label class="config-label">Members</label></td>
      {#each gmu as n, i}
        {#if i > 0}
           <td><input id="gmu{i}" style="width:2em;" type="number" min="0" max="7" value="{n}"></td>
        {/if}
      {/each}
    </tr>
  </table>

  <br>
  <button id="crlb" type="button" on:click={hClick_Reload}>Reload</button>
  <button id="csvb" type="button" on:click={hClick_Save}>Save</button>
  <br><br>
  <button id="mrtb" type="button" on:click={hClick_RestartMcu}>Restart MCU</button><br>
  <div id="config_restart_div"></div>

  <section>
    <h1>Configuration-Wizards</h1>
    <ul>
      <li><button id="id_cuasb" type="button" on:click={hClick_ScanCU}>Scan Central-Unit-ID</button> <span id="id_cuasStatus"></span></li>
    </ul>
  </section>
</div>
