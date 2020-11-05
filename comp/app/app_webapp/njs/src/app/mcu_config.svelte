<script>
  "use strict";
  import { _ } from "services/i18n";
  import { McuConfig, McuConfigKeys, Gmu, McuGpiosFree } from "stores/mcu_config.js";
  import * as appDebug from "app/app_debug.js";
  import * as httpFetch from "app/fetch.js";
  import * as cuas from "app/cuas.js";
  import * as misc from "app/misc.js";
  import { onMount, onDestroy } from "svelte";
  import { ReloadProgress } from "stores/app_state.js";

  import McuConfigGpio from "components/mcu_config/gpio.svelte";
  import McuConfigGpioSelect from "components/mcu_config/gpio_select.svelte";
  import McuConfigNetwork from "components/mcu_config/network.svelte";
  import McuConfigLanPhy from "components/mcu_config/lan_phy.svelte";
  import McuConfigNumber from "components/mcu_config/number.svelte";
  import McuConfigEnable from "components/mcu_config/enable.svelte";
  import McuConfigAstroCorrection from "components/mcu_config/astro_correction.svelte";
  import McuConfigUsedMembers from "components/mcu_config/used_members.svelte";

  import GpioLevel from "components/gpio_level.svelte"

  let on_destroy = [];
  onMount(() => {
    console.log("fetch_config");
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG | httpFetch.FETCH_CONFIG_GPIO_STRING);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: mcuConfigKeys = $McuConfigKeys;

  let mcuConfig = {};
  function updateMcuConfig(obj) {
    mcuConfig = { ...obj };
    console.log("mcuConfig: ", mcuConfig);
  }

  $: {
    updateMcuConfig($McuConfig);
  }

  $: gmu = $Gmu;
  $: gpios = $McuConfig["gpio"] || "..........................................";

  export function reload_config() {
    updateMcuConfig($McuConfig);

    for (let i = 1; i <= 7; ++i) {
      document.getElementById("gmu" + i.toString()).value = $Gmu[i];
    }
  }

  function hClick_Reload() {
    reload_config();
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
  }

  function hClick_Save() {
    mcuConfig_saveModifiedOptions();
  }

  function hClick_RestartMcu() {
    misc.req_mcuRestart();
  }

  function hClick_ScanCU() {
    cuas.req_cuasStart();
  }

  function mcuConfig_getCfgMod() {
    const cfg_mcu = $McuConfig;
    const cfg_ui = mcuConfig;
    let cfg_mod = {};
    let hasModified = false;

    Object.keys(cfg_ui).forEach(function (key, idx) {
      const val_ui = cfg_ui[key];
      if (cfg_mcu[key] !== val_ui) {
        hasModified = true;
        if (typeof val_ui === "boolean") {
          cfg_mod[key] = val_ui ? 1 : 0;
        } else {
          cfg_mod[key] = cfg_ui[key];
        }
      }
    });
    return hasModified ? cfg_mod : null;
  }

  function mcuConfig_saveModifiedOptions() {
    let cfg_mod = mcuConfig_getCfgMod();
    if (cfg_mod === null) return;

    Object.keys(cfg_mod).forEach(function (key, idx) {
      if (key.startsWith("gpio") && key !== "gpio" && cfg_mod[key] === "d") {
        console.log("remove key: ", key);
        McuConfig.remove(key);
      }
    });

    cfg_mod.all = "?";
    let url = "/cmd.json";
    httpFetch.http_postRequest(url, { config: cfg_mod });
  }

  ///////////////// wizard ////////////////////
  $: wiz_gpio = -1;
  $: wiz_gpio_status = "";

  function wiz_addGpio() {
    if (wiz_gpio === -1)
       return;
    let cmd = { to: "tfmcu", config: {} };
    const gpioKey = "gpio" + wiz_gpio;
    cmd.config[gpioKey] = "i";
    wiz_gpio = -1;

    httpFetch.http_postRequest("/cmd.json", cmd);
    wiz_gpio_status = "";
    setTimeout(() => {
      httpFetch.http_postRequest("/cmd.json", { config: { gpio: "?" } });
      httpFetch.http_postRequest("/cmd.json", { config: { gpio: "$" } });
      setTimeout(() => {
        wiz_gpio_status = $McuConfig[gpioKey] ? '<span class="bg-green-500">ok</span>' : '<span class="bg-red-500">error</span>';
      }, 200);
    }, 500);
  }

  /////////////////////////////////////////////
</script>

<style type="text/scss">
  @import "../styles/app.scss";
  .row1 {
    background-color: darken($color_bg_area, 10%);
  }
  .row2 {
    background-color: lighten($color_bg_area, 10%);
  }
</style>

<div class="config" id="config-div">
  <table id="cfg_table_id" class="conf-table top_table">
    {#each mcuConfigKeys as name, i}
      <tr class={i % 2 ? 'row1' : 'row2'}>
        {#if name !== 'gm-used'}
          <td><label class="config-label {mcuConfig[name] != $McuConfig[name] ? 'font-bold' : ''}" for="cfg_{name}">{name}</label></td>
        {/if}

        {#if name.endsWith('-enable')}
          <td>
            <McuConfigEnable {name} bind:value={mcuConfig[name]} />
          </td>
        {:else if name === 'latitude'}
          <td>
            <McuConfigNumber {name} bind:value={mcuConfig[name]} min="-90" max="90" step="0.01" />
          </td>
        {:else if name === 'longitude'}
          <td>
            <McuConfigNumber {name} bind:value={mcuConfig[name]} min="-180" max="180" step="0.01" />
          </td>
        {:else if name === 'rf-rx-pin' || name === 'set-button-pin'}
          <td>
            <McuConfigGpioSelect {name} bind:value={mcuConfig[name]} />
          </td>
        {:else if name === 'rf-tx-pin'}
          <td>
            <McuConfigGpioSelect {name} bind:value={mcuConfig[name]} max="36"/>
          </td>
        {:else if name === 'verbose'}
          <td>
            <McuConfigNumber {name} bind:value={mcuConfig[name]} min="0" max="5" />
          </td>
        {:else if name === 'network'}
          <td>
            <McuConfigNetwork {name} bind:value={mcuConfig[name]} />
          </td>
        {:else if name === 'lan-phy'}
          <td>
            <McuConfigLanPhy {name} bind:value={mcuConfig[name]} />
          </td>
        {:else if name === 'lan-pwr-gpio'}
          <td>
            <McuConfigGpioSelect {name} bind:value={mcuConfig[name]} max="36"/>
          </td>
        {:else if name === 'astro-correction'}
          <td>
            <McuConfigAstroCorrection {name} bind:value={mcuConfig[name]} />
          </td>
        {:else if name.startsWith('gpio')}
          <td>
            <McuConfigGpio {name} bind:value={mcuConfig[name]} /> <GpioLevel {name} />
          </td>
        {:else if name !== 'gm-used'}
          <td><input class="config-input text" type="text" id="cfg_{name}" {name} bind:value={mcuConfig[name]} /></td>
        {/if}
      </tr>
    {/each}
  </table>
</div>

{#if 'gm-used' in mcuConfig}
  <McuConfigUsedMembers name="gm-used" bind:value={mcuConfig['gm-used']} changed={mcuConfig['gm-used'] !== $McuConfig['gm-used']} />
{/if}
<button type="button" on:click={hClick_Reload}>{$_('app.reload')}</button>
<button type="button" on:click={hClick_Save}>{$_('app.save')}</button>
<button type="button" on:click={hClick_RestartMcu}> {$_('app.restartMcu')} </button>

{#if $ReloadProgress > 0}
  <br />
  <strong>{$_('app.msg_waitForMcuRestart')}</strong>
  <br />
  <progress id="reload_progress_bar" value={$ReloadProgress} max="100" />
{/if}

<section>
  <h4>Configuration-Wizards</h4>
  <ul>
    <li>
      <label>GPIO add: 
        <McuConfigGpioSelect name="wizard_gpio" bind:value={wiz_gpio} show_value={false}/>
        <button type="button" on:click={wiz_addGpio}>Add</button>
        {@html wiz_gpio_status}
      </label>
    </li>

    <li><button id="id_cuasb" type="button" on:click={hClick_ScanCU}> {$_('app.wizard_cuas')} </button> <span id="id_cuasStatus" /></li>
  </ul>
</section>
