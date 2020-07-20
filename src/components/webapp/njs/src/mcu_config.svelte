<script>
  "use strict";
  import { _ } from './services/i18n';
  import { McuConfig, McuConfigKeys, Gmu } from "./store/mcu_config.js";
  import * as appDebug from "./app_debug.js";
  import * as httpFetch from "./fetch.js";
  import * as cuas from "./cuas.js";
  import * as misc from "./misc.js";
  import { onMount, onDestroy } from "svelte";
  import { ReloadProgress } from './store/app_state.js';

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
    Object.keys($McuConfig).forEach(function (key, idx) {
      let el = document.getElementById("cfg_" + key);

      switch (el.type) {
        case "checkbox":
          el.checked = $McuConfig[key] !== 0;
          break;
        default:
          el.value = $McuConfig[key];
      }
    });

    for (let i = 1; i <= 7; ++i) {
      document.getElementById("gmu" + i.toString()).value = $Gmu[i];
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
    Object.keys(cfg).forEach(function (key, idx) {
      let new_val = 0;
      let el = document.getElementById("cfg_" + key);
      appDebug.dbLog("key: " + key);

      switch (el.type) {
        case "checkbox":
          new_val = el.checked ? 1 : 0;
          break;
        case "number":
          new_val = el.valueAsNumber;
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
      let url = "/cmd.json";
      httpFetch.http_postRequest(url, { config: new_cfg });
    }
  }

  function usedMembers_fromHtml_toHtml() {
    let val = "";

    let written = false; // to skip leading zeros
    for (let i = 7; i >= 1; --i) {
      let id = "gmu" + i.toString();
      let mct = document.getElementById(id).value;
      if (mct !== "0" || written) {
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
  .conf-table label {
    padding: 2px;
    margin-right: 3px;
  }
  .config-input.text {
    width: inherit;
  }
  .config-input.number {
    width: 5rem;
  }
</style>

<div id="configdiv" class="top-div">
  <div class="config" id="config-div">
    <table id="cfg_table_id" class="conf-table top_table">
      {#each mcuConfigKeys as name, i}
        <tr class={i % 2 ? 'bg-yellow-200' : 'bg-yellow-300'}>

          {#if name !== 'gm-used'}
            <td>
              <label class="config-label">{name}</label>
            </td>
          {/if}

          {#if name.endsWith('-enable')}
            <td>
              <input
                class="config-input cb"
                type="checkbox"
                id="cfg_{name}"
                {name}
                checked={mcuConfig[name]} />
            </td>
          {:else if name === 'latitude'}
            <td>
              <input
                class="config-input number"
                type="number"
                min="-90"
                max="90"
                step="0.01"
                id="cfg_{name}"
                {name}
                value={mcuConfig[name]} />
            </td>
          {:else if name === 'longitude'}
            <td>
              <input
                class="config-input number"
                type="number"
                min="-180"
                max="180"
                step="0.01"
                id="cfg_{name}"
                {name}
                value={mcuConfig[name]} />
            </td>
          {:else if name === 'rf-rx-pin' || name === 'set-button-pin'}
            <td>
              <input
                class="config-input number"
                type="number"
                min="-1"
                max="39"
                id="cfg_{name}"
                {name}
                value={mcuConfig[name]} />
            </td>
          {:else if name === 'rf-tx-pin'}
            <td>
              <input
                class="config-input number"
                type="number"
                min="-1"
                max="33"
                id="cfg_{name}"
                {name}
                value={mcuConfig[name]} />
            </td>
          {:else if name === 'verbose'}
            <td>
              <input
                class="config-input number"
                type="number"
                min="0"
                max="5"
                id="cfg_{name}"
                {name}
                value={mcuConfig[name]} />
            </td>
          {:else if name === 'network'}
            <td>
              <select class="config-input" id="cfg_{name}" value={mcuConfig[name]}>
                <option value="wlan">Existing WLAN</option>
                <option value="ap">WLAN Accesspoint</option>
                <option value="lan">Ethernet</option>
                <!-- dev-no-lan-delete-line -->
                <option value="none">No Network</option>
              </select>
            </td>
          {:else if name === 'lan-phy'}
            <td>
              <select class="config-input" id="cfg_{name}" value={mcuConfig[name]}>
                <option value="lan8270">LAN8270</option>
                <option value="rtl8201">RTL8201</option>
                <option value="ip101">IP101</option>
              </select>
            </td>
          {:else if name === 'lan-pwr-gpio'}
            <td>
              <input
                class="config-input number"
                type="number"
                min="-1"
                max="36"
                id="cfg_{name}"
                {name}
                value={mcuConfig[name]} />
            </td>
          {:else if name === 'astro-correction'}
            <td>
              <select class="config-input" id="cfg_{name}">
                <option value="0">average</option>
                <option value="1">not too late or dark</option>
                <option value="2">not too early or bright</option>
              </select>
            </td>
          {:else if name.startsWith('gpio')}
            <td>
              <select class="config-input" id="cfg_{name}" value={mcuConfig[name]}>
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
                <option value="qh">
                  Input/Output (OpenDrain + Level.High)
                </option>
              </select>
            </td>
          {:else if name !== 'gm-used'}
            <td>
              <input
                class="config-input text"
                type="text"
                id="cfg_{name}"
                {name}
                value={mcuConfig[name]} />
            </td>
          {/if}

        </tr>
      {/each}

    </table>

  </div>

  <input
    type="text"
    id="cfg_gm-used"
    value={mcuConfig['gm-used']}
    style="display:none;" />

  <table id="gmu-table">
    <tr>
      <td>
        <label class="config-label">Groups</label>
      </td>
      {#each { length: 7 } as _, i}
        <td>
          <label>{i + 1}</label>
        </td>
      {/each}
    </tr>
    <tr>
      <td>
        <label class="config-label">Members</label>
      </td>
      {#each gmu as n, i}
        {#if i > 0}
          <td>
            <input
              id="gmu{i}"
              style="width:2em;"
              type="number"
              min="0"
              max="7"
              value={n} />
          </td>
        {/if}
      {/each}
    </tr>
  </table>

  <br />
  <button id="crlb" type="button" on:click={hClick_Reload}>{$_('app.reload')}</button>
  <button id="csvb" type="button" on:click={hClick_Save}>{$_('app.save')}</button>
  <br />
  <br />
  <button id="mrtb" type="button" on:click={hClick_RestartMcu}>
    {$_('app.restartMcu')}
  </button>
  <br />
  <div id="config_restart_div" />
  {#if $ReloadProgress > 0}
    <strong>>$_('app.msg_waitForMcuRestart')</strong><br>
    <progress id="reload_progress_bar" value="{$ReloadProgress}" max="100"></progress>
  {/if}
  <section>
    <h1>Configuration-Wizards</h1>
    <ul>
      <li>
        <button id="id_cuasb" type="button" on:click={hClick_ScanCU}>
          {$_('app.wizard_cuas')}
        </button>
        <span id="id_cuasStatus" />
      </li>
    </ul>
  </section>
</div>
