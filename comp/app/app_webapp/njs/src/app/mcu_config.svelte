<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import { McuConfig, McuConfigKeys, Gmu, McuGpiosFree } from "stores/mcu_config.js";
  import { McuDocs, McuDocs_cliHelpConfig } from "stores/mcu_docs.js";
  import * as appDebug from "app/app_debug.js";
  import * as httpFetch from "app/fetch.js";
  import * as cuas from "app/cuas.js";
  import * as misc from "app/misc.js";
  import { onMount, onDestroy } from "svelte";
  import { ReloadProgress } from "stores/app_state.js";
  import NavTabs from "app/nav_tabs.svelte";
  import { TabIdx } from "stores/app_state.js";
  import { McuErrorMask } from "stores/mcu_firmware.js";

  import McuConfigGpio from "components/mcu_config/gpio.svelte";
  import McuConfigGpioSelect from "components/mcu_config/gpio_select.svelte";
  import McuConfigNetwork from "components/mcu_config/network.svelte";
  import McuConfigLanPhy from "components/mcu_config/lan_phy.svelte";
  import McuConfigNumber from "components/mcu_config/number.svelte";
  import McuConfigEnable from "components/mcu_config/enable.svelte";
  import McuConfigAstroCorrection from "components/mcu_config/astro_correction.svelte";
  import McuConfigUsedMembers from "components/mcu_config/used_members.svelte";
  import McuRfTrx from "components/mcu_config/rf_trx.svelte";
  import GpioLevel from "components/gpio_level.svelte";
  import CC1101 from "panes/cc1101.svelte";
  import AppLog from "panes/app_log.svelte";

  let on_destroy = [];

  function fetch_one_after_another() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG_P0, true);
    const timeout = 250;
    setTimeout(() => {
      httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG_P1, true);
      setTimeout(() => {
        httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG_P2, true);
        setTimeout(() => {
          httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG_P3 | httpFetch.FETCH_ERROR_MASK, true);
        }, timeout);
        setTimeout(() => {
          httpFetch.getFile("/f/cli/help/config");
        }, timeout);
      }, timeout);
    }, timeout);
    



  
  }

  onMount(() => {
    fetch_one_after_another();
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: {
    console.log("McuDocs_cliHelpConfig", $McuDocs_cliHelpConfig, $McuDocs_cliHelpConfig["network"]);
  }
  $: tabIdxMcc = $TabIdx["mcc"] || 0;

  $: mcuConfigNames = {}; // localized GUI labels
  $: {
    for (let key of $McuConfigKeys) {
      mcuConfigNames[key] = $_({ id: "mcuConfigNames." + key, default: key });
    }
  }

  let mcuConfig = {};
  function updateMcuConfig(obj) {
    mcuConfig = { ...obj };
  }

  $: {
    updateMcuConfig($McuConfig);
  }

  $: hasCc1101 = mcuConfig["rf-trx"] === "cc1101";

  $: errorBit_cc1101_gpio_rfin = ($McuErrorMask & 0x01) !== 0;

  $: mcuConfigKeysNetwork = $McuConfigKeys.filter((val) => val === "network");
  $: mcuConfigKeysMQTT = $McuConfigKeys.filter((val) => val.startsWith("mqtt-"));
  $: mcuConfigKeysHTTP = $McuConfigKeys.filter((val) => val.startsWith("http-"));
  $: mcuConfigKeysWLAN = $McuConfigKeys.filter((val) => val.startsWith("wlan-"));
  $: mcuConfigKeysLAN = $McuConfigKeys.filter((val) => val.startsWith("lan-"));
  $: mcuConfigKeysNTP = $McuConfigKeys.filter((val) => val.startsWith("ntp-"));
  $: mcuConfigKeysCC1101 = $McuConfigKeys.filter((val) => val.startsWith("cc1101-"));

  $: mcuConfigKeysPin = $McuConfigKeys.filter(
    (val) => val.startsWith("gpio") || val === "set-button-pin" || (mcuConfig["rf-trx"] !== "cc1101" && (val === "rf-rx-pin" || val === "rf-tx-pin"))
  );
  $: mcuConfigKeysCU = $McuConfigKeys.filter((val) => val === "cu");
  $: mcuConfigKeysAstro = $McuConfigKeys.filter((val) => val === "longitude" || val === "latitude" || val.startsWith("astro-"));
  $: mcuConfigKeysTime = $McuConfigKeys.filter((val) => val === "rtc" || val === "tz");
  $: mcuConfigKeysCc1101Pin = mcuConfig["rf-trx"] === "cc1101" ? $McuConfigKeys.filter((val) => val.startsWith("rf-") && val.endsWith("-pin")) : [];
  $: mcuConfigKeysIgnore = ["rf-repeater"];

  $: mcuConfigKeysMisc = $McuConfigKeys.filter(
    (val) =>
      !(
        val === "gm-used" ||
        mcuConfigKeysNetwork.includes(val) ||
        mcuConfigKeysMQTT.includes(val) ||
        mcuConfigKeysHTTP.includes(val) ||
        mcuConfigKeysWLAN.includes(val) ||
        mcuConfigKeysLAN.includes(val) ||
        mcuConfigKeysNTP.includes(val) ||
        mcuConfigKeysCC1101.includes(val) ||
        mcuConfigKeysCU.includes(val) ||
        mcuConfigKeysAstro.includes(val) ||
        mcuConfigKeysTime.includes(val) ||
        mcuConfigKeysCc1101Pin.includes(val) ||
        mcuConfigKeysIgnore.includes(val) ||
        val.endsWith("-pin") ||
        mcuConfigKeysPin.includes(val)
      )
  );

  $: {
    console.log("mcuConfigKeysCc1101Pin: ", mcuConfigKeysCc1101Pin);
    console.log("mcuConfigKeysMisc: ", mcuConfigKeysMisc);
    console.log("$McuConfigKeys: ", $McuConfigKeys);
    console.log("rf-trx: ", mcuConfig["rf-trx"]);
    console.log("mcuConfig: ", mcuConfig);
  }

  $: gmu = $Gmu;
  $: gpios = $McuConfig["gpio"] || "..........................................";

  export function reload_config() {
    updateMcuConfig($McuConfig);
  }

  function hClick_Reload() {
    reload_config();
    fetch_one_after_another();
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
        McuConfig.remove(key);
      }
    });

    httpFetch.http_postCommand({ config: cfg_mod });

    setTimeout(() => {
      fetch_one_after_another();
    }, 500);

    setTimeout(() => {
      fetch_one_after_another();
    }, 2500);
  }

  function cfg_by_name(name) {
    let res = {};
    res.name = name;
    return res;
  }

  ///////////////// wizard ////////////////////
  $: wiz_gpio = -1;
  $: wiz_gpio_status = "";

  function wiz_addGpio() {
    if (wiz_gpio === -1) return;
    let cmd = { to: "tfmcu", config: {} };
    const gpioKey = "gpio" + wiz_gpio;
    cmd.config[gpioKey] = "i";
    wiz_gpio = -1;

    httpFetch.http_postCommand(cmd);
    wiz_gpio_status = "";
    setTimeout(() => {
      httpFetch.http_postCommand({ config: { gpio: "?" } });
      httpFetch.http_postCommand({ config: { gpio: "$" } });
      setTimeout(() => {
        wiz_gpio_status = $McuConfig[gpioKey] ? '<span class="bg-green-500">ok</span>' : '<span class="bg-red-500">error</span>';
      }, 200);
    }, 500);
  }

  /////////////////////////////////////////////
</script>

<NavTabs nav_tabs={[$_("mcuConfig.network"), $_("mcuConfig.misc"), ...(hasCc1101 ? ["CC1101"] : [])]} name="mcc" vertical={true} />


{#if tabIdxMcc === 0}
  {#if mcuConfigKeysNetwork.length > 0}
    <div class="area network">
      <table class="conf-table top_table rounded-xl overflow-hidden">
        {#each mcuConfigKeysNetwork as key, i}
          <tr>
            <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}" >{mcuConfigNames[key]}</label></td>
            {#if key.endsWith("-enable")}
              <td>
                <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else if key === "network"}
              <td>
                <McuConfigNetwork name={key} bind:value={mcuConfig[key]} has_lan={mcuConfigKeysLAN.length > 0 || mcuConfig.network === "lan"} />
              </td>
            {:else}
              <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
            {/if}
          </tr>
        {/each}
      </table>

      {#if mcuConfig.network !== "none"}
        {#if mcuConfigKeysWLAN.length > 0 && mcuConfig.network === "wlan"}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.wlan_station")}</caption>
              {#each mcuConfigKeysWLAN as key, i}
                <tr>
                  <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
                  {#if key.endsWith("-enable")}
                    <td>
                      <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                    </td>
                  {:else}
                    <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                  {/if}
                </tr>
              {/each}
            </table>
          </div>
        {/if}

        {#if mcuConfigKeysLAN.length > 0 && mcuConfig.network === "lan"}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.ethernet")}</caption>
              {#each mcuConfigKeysLAN as key, i}
                <tr>
                  <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
                  {#if key.endsWith("-enable")}
                    <td>
                      <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                    </td>
                  {:else if key === "lan-phy"}
                    <td>
                      <McuConfigLanPhy name={key} bind:value={mcuConfig[key]} />
                    </td>
                  {:else if key === "lan-pwr-gpio"}
                    <td>
                      <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} max="36" />
                    </td>
                  {:else}
                    <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                  {/if}
                </tr>
              {/each}
            </table>
          </div>
        {/if}

        {#if mcuConfigKeysNTP.length > 0}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.ntp_client")}</caption>
              {#each mcuConfigKeysNTP as key, i}
                <tr>
                  <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
                  {#if key.endsWith("-enable")}
                    <td>
                      <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                    </td>
                  {:else}
                    <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                  {/if}
                </tr>
              {/each}
            </table>
          </div>
        {/if}

        {#if mcuConfigKeysMQTT.length > 0}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.mqtt_client")}</caption>
              {#each mcuConfigKeysMQTT as key, i}
                <tr>
                  <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>

                  {#if key.endsWith("-enable")}
                    <td>
                      <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                    </td>
                  {:else}
                    <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                  {/if}
                </tr>
              {/each}
            </table>
          </div>
        {/if}

        {#if mcuConfigKeysHTTP.length > 0}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.http_server")}</caption>
              {#each mcuConfigKeysHTTP as key, i}
                <tr>
                  <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>

                  {#if key.endsWith("-enable")}
                    <td>
                      <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                    </td>
                  {:else}
                    <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                  {/if}
                </tr>
              {/each}
            </table>
          </div>
        {/if}
      {/if}
    </div>
  {/if}
{:else if tabIdxMcc === 1}
  <div class="area network">
    {#if "gm-used" in mcuConfig}
      <div class="area">
        <McuConfigUsedMembers
          caption={$_("mcuConfig.gm_used")}
          name={mcuConfigNames["gm-used"]}
          bind:value={mcuConfig["gm-used"]}
          changed={mcuConfig["gm-used"] !== $McuConfig["gm-used"]}
        />
      </div>
    {/if}

    {#if mcuConfigKeysCU.length > 0}
      <div class="area">
        <table class="conf-table top_table rounded-xl overflow-hidden">
          {#each mcuConfigKeysCU as key, i}
            <tr>
              <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
              {#if key.endsWith("-enable")}
                <td>
                  <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else}
                <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
              {/if}
            </tr>
          {/each}
        </table>
        <div class="area"><button id="id_cuasb" type="button" on:click={hClick_ScanCU}> {$_("app.wizard_cuas")} </button> <span id="id_cuasStatus" /></div>
      </div>
    {/if}

    {#if mcuConfigKeysTime.length > 0}
      <div class="area">
        <table class="conf-table top_table rounded-xl overflow-hidden">
          {#each mcuConfigKeysTime as key, i}
            <tr>
              <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
              {#if key.endsWith("-enable")}
                <td>
                  <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else}
                <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
              {/if}
            </tr>
          {/each}
        </table>
      </div>
    {/if}

    {#if mcuConfigKeysAstro.length > 0}
      <div class="area">
        <table class="conf-table top_table rounded-xl overflow-hidden">
          {#each mcuConfigKeysAstro as key, i}
            <tr>
              <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
              {#if key.endsWith("-enable")}
                <td>
                  <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else if key === "latitude"}
                <td>
                  <McuConfigNumber name={key} bind:value={mcuConfig[key]} min="-90" max="90" step="0.01" />
                </td>
              {:else if key === "longitude"}
                <td>
                  <McuConfigNumber name={key} bind:value={mcuConfig[key]} min="-180" max="180" step="0.01" />
                </td>
              {:else if key === "astro-correction"}
                <td>
                  <McuConfigAstroCorrection name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else}
                <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
              {/if}
            </tr>
          {/each}
        </table>
      </div>
    {/if}

    <div class="area">
      <table class="conf-table top_table rounded-xl overflow-hidden">
        <caption>{$_("mcuConfig.pin_gpio")}</caption>

        {#each mcuConfigKeysCc1101Pin as key, i}
          <tr>
            <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
              ><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{$_("mcuConfigNames.cc1101." + key)}</label
              ></td
            >
            {#if key === "rf-rx-pin"}
              <td>
                <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} />
                {#if errorBit_cc1101_gpio_rfin}
                  <span class="bg-red-400">(wire not connected)</span>
                {/if}
              </td>
            {:else if key === "set-button-pin" || key === "rf-miso-pin"}
              <td>
                <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else if key === "rf-tx-pin" || key === "rf-sclk-pin" || key == "rf-mosi-pin" || key === "rf-ss-pin"}
              <td>
                <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} max="33" />
              </td>
            {:else}
              <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
            {/if}
          </tr>
        {/each}

        {#each mcuConfigKeysPin as key, i}
          <tr>
            <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
            {#if key.endsWith("-enable")}
              <td>
                <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else if key === "rf-rx-pin" || key === "set-button-pin"}
              <td>
                <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else if key === "rf-tx-pin"}
              <td>
                <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} max="33" />
              </td>
            {:else if key.startsWith("gpio")}
              <td>
                <McuConfigGpio name={key} bind:value={mcuConfig[key]} />
                <GpioLevel name={key} />
              </td>
            {:else if key === "rf-rx-pin" || key === "set-button-pin" || key === "rf-miso-pin"}
              <td>
                <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else if key === "rf-tx-pin" || key === "rf-sclk-pin" || key == "rf-mosi-pin" || key === "rf-ss-pin"}
              <td>
                <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} max="33" />
              </td>
            {:else}
              <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
            {/if}
          </tr>
        {/each}
      </table>
      <div class="area">
        <label
          >GPIO add:
          <McuConfigGpioSelect name="wizard_gpio" bind:value={wiz_gpio} show_value={false} />
          <button type="button" on:click={wiz_addGpio}>Add</button>
          {@html wiz_gpio_status}
        </label>
      </div>
    </div>

    <div class="area" id="config-div">
      <table id="cfg_table_id" class="conf-table top_table rounded-xl overflow-hidden">
        {#each mcuConfigKeysMisc as key, i}
          <tr>
            <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
            {#if key.endsWith("-enable")}
              <td>
                <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else if key === "verbose"}
              <td>
                <McuConfigNumber name={key} bind:value={mcuConfig[key]} min="0" max="5" />
              </td>
            {:else if key === "rf-trx"}
              <td>
                <McuRfTrx name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else}
              <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
            {/if}
          </tr>
        {/each}
      </table>
    </div>
  </div>
{:else if tabIdxMcc === 2}
  {#if mcuConfigKeysCC1101.length}
    <div class="area">
      {#each mcuConfigKeysCC1101 as key, i}
        <tr>
          <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}><label class="config-label {mcuConfig[key] != $McuConfig[key] ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label></td>
          {#if key.endsWith("-enable")}
            <td>
              <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
            </td>
          {:else}
            <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
          {/if}
        </tr>
      {/each}
    </div>
  {/if}
{/if}

<button type="button" on:click={hClick_Reload}>{$_("app.reload")}</button>
<button type="button" on:click={hClick_Save}>{$_("app.save")}</button>
<button type="button" on:click={hClick_RestartMcu}> {$_("app.restartMcu")}</button>

{#if tabIdxMcc === 2}
  <hr />
  <CC1101 />
{/if}

{#if $ReloadProgress > 0}
  <br />
  <strong>{$_("app.msg_waitForMcuRestart")}</strong>
  <br />
  <progress id="reload_progress_bar" value={$ReloadProgress} max="100" />
{/if}

{#if tabIdxMcc === 2}
  <hr />
  <h3>Receiver Log</h3>
  <AppLog rxonly={true} />
{/if}

<style type="text/scss">
  @import "../styles/app.scss";
  .row1 {
    background-color: darken($color_bg_area, 10%);
  }
  .row2 {
    background-color: lighten($color_bg_area, 10%);
  }
  .network {
    background-color: darken($color_bg_area, 5%);
  }
</style>
