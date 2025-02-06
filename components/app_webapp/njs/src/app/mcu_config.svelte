<script>
  ("use strict");
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { GuiAcc, TabIdx } from "../store/app_state";
  import { McuConfig, McuConfigKeys, Gmu, Cuas } from "../store/mcu_config.js";
  import { McuDocs_cliHelpConfig } from "../store/mcu_docs.js";
  import * as httpFetch from "../app/fetch.js";
  import * as cuas from "../app/cuas.js";
  import * as misc from "../app/misc.js";
  import { onMount, onDestroy } from "svelte";
  import { ReloadProgress } from "../store/app_state.js";
  import NavTabs from "../app/nav_tabs.svelte";
  import { McuErrorBits, McuError_cc1101_gdo2_nc, McuError_cc1101_init } from "../store/mcu_firmware.js";

  import McuConfigGpio from "../components/mcu_config/gpio.svelte";
  import McuConfigGpioSelect from "../components/mcu_config/gpio_select.svelte";
  import McuConfigNetwork from "../components/mcu_config/network.svelte";
  import McuConfigLanPhy from "../components/mcu_config/lan_phy.svelte";
  import McuConfigNumber from "../components/mcu_config/number.svelte";
  import McuConfigEnable from "../components/mcu_config/enable.svelte";
  import McuConfigAstroCorrection from "../components/mcu_config/astro_correction.svelte";
  import McuConfigUsedMembers from "../components/mcu_config/used_members.svelte";
  import McuRfTrx from "../components/mcu_config/rf_trx.svelte";
  import GpioLevel from "../components/gpio_level.svelte";
  import CC1101 from "../panes/cc1101.svelte";
  import AppLog from "../panes/app_log.svelte";

  let on_destroy = [];

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG | httpFetch.FETCH_ERROR_MASK);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $effect(() => {
    console.log("McuDocs_cliHelpConfig", $McuDocs_cliHelpConfig, $McuDocs_cliHelpConfig["network"]);
  });
  let tabIdxMcc = $derived($TabIdx["mcc"] || 0);

  let mcuConfigNames = $derived(makeMcuConfigNames($McuConfigKeys)); // localized GUI labels
  function makeMcuConfigNames(mcuCfgKeys) {
    let result = {};
    for (let key of mcuCfgKeys) {
      result[key] = $_({ id: "mcuConfigNames." + key, default: key });
    }
    return result;
  }

  let mcuConfig = $state({...$McuConfig});
  function updateMcuConfig(obj) {
    mcuConfig = { ...obj };
  }

  $effect(() => {
    updateMcuConfig($McuConfig);
  });

  let hasCc1101 = $derived(mcuConfig["rf-trx"] === "cc1101");

  let mcuConfigKeysNetwork = $derived($McuConfigKeys.filter((val) => val === "network"));
  let mcuConfigKeysMQTT = $derived($McuConfigKeys.filter((val) => val.startsWith("mqtt-")));
  let mcuConfigKeysHTTP = $derived($McuConfigKeys.filter((val) => val.startsWith("http-")));
  let mcuConfigKeysWLAN = $derived($McuConfigKeys.filter((val) => val.startsWith("wlan-")));
  let mcuConfigKeysLAN = $derived($McuConfigKeys.filter((val) => val.startsWith("lan-")));
  let mcuConfigKeysNTP = $derived($McuConfigKeys.filter((val) => val.startsWith("ntp-")));
  let mcuConfigKeysCC1101 = $derived($McuConfigKeys.filter((val) => val.startsWith("cc1101-")));

  let mcuConfigKeysPin = $derived(
    $McuConfigKeys.filter(
      (val) =>
        val.startsWith("gpio") ||
        val === "set-button-pin" ||
        (mcuConfig["rf-trx"] !== "cc1101" && mcuConfig["rf-trx"] !== "none" && (val === "rf-rx-pin" || val === "rf-tx-pin"))
    )
  );
  let mcuConfigKeysCU = $derived($McuConfigKeys.filter((val) => val === "cu"));
  let mcuConfigKeysAstro = $derived($McuConfigKeys.filter((val) => val === "longitude" || val === "latitude" || val.startsWith("astro-")));
  let mcuConfigKeysTime = $derived($McuConfigKeys.filter((val) => val === "rtc" || val.startsWith("rtc-") || val === "tz"));
  let mcuConfigKeysCc1101Pin = $derived(mcuConfig["rf-trx"] === "cc1101" ? $McuConfigKeys.filter((val) => val.startsWith("rf-") && val.endsWith("-pin")) : []);
  let mcuConfigKeysIgnore = $derived(["rf-repeater"]);

  let mcuConfigKeysMisc = $derived(
    $McuConfigKeys.filter(
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
    )
  );

  $effect(() => {
    console.log("mcuConfigKeysCc1101Pin: ", mcuConfigKeysCc1101Pin);
    console.log("mcuConfigKeysMisc: ", mcuConfigKeysMisc);
    console.log("$McuConfigKeys: ", $McuConfigKeys);
    console.log("rf-trx: ", mcuConfig["rf-trx"]);
    console.log("mcuConfig: ", mcuConfig);
  });

  let gmu = $derived($Gmu);
  let gpios = $derived($McuConfig["gpio"] || "..........................................");

  export function reload_config() {
    updateMcuConfig($McuConfig);
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

    McuConfig.update(mcuConfig);
    Object.keys(cfg_mod).forEach(function (key, idx) {
      if (key.startsWith("gpio") && key !== "gpio" && cfg_mod[key] === "d") {
        McuConfig.remove(key);
      }
    });

    httpFetch.http_postCommand({ config: cfg_mod });

    setTimeout(() => {
      httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
    }, 500);

    setTimeout(() => {
      httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
    }, 2500);
  }

  function cfg_by_name(name) {
    let res = {};
    res.name = name;
    return res;
  }

  function hasKeyEdited(key, val1, val2) {
    if (val1 === val2) return false;
    if (key.endsWith("-enable") && !val1 === !val2) return false;
    return true;
  }

  ///////////////// wizard ////////////////////
  let wiz_gpio = $state(-1);

  let wiz_gpio_status = $state("");

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

<div class="navtab-sub2 flex flex-col items-center px-1 border-none">
  <NavTabs
    nav_tabs={[
      { name: $_("navTab.cfg.mcu.network.tab"), idx: 0 },
      { name: $_("navTab.cfg.mcu.misc.tab"), idx: 1 },
      ...($GuiAcc.edit_rf_module && hasCc1101 && !$McuError_cc1101_init ? [{ name: "CC1101", idx: 2 }] : []),
    ]}
    name="mcc"
    vertical={false}
  />
</div>

<div class="main-area">
  {#if tabIdxMcc === 0}
    {#if mcuConfigKeysNetwork.length > 0}
      <h4 class="text-center" use:tippy={{ content: $_("panes.network.tt.header") }}>{$_("panes.network.header")}</h4>
      <table class="conf-table top_table rounded-xl overflow-hidden">
        <tbody>
          {#each mcuConfigKeysNetwork as key, i}
            <tr>
              <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                  >{mcuConfigNames[key]}</label
                ></td
              >
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
        </tbody>
      </table>

      {#if mcuConfigKeysLAN.length > 0 && (mcuConfig.network === "lan" || mcuConfig.network === "lan-wlan")}
        <div class="area">
          <table class="conf-table top_table rounded-xl overflow-hidden">
            <caption>{$_("mcuConfig.ethernet")}</caption>
            <tbody>
              {#each mcuConfigKeysLAN as key, i}
                <tr>
                  <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                    ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                      >{mcuConfigNames[key]}</label
                    ></td
                  >
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
                      <McuConfigGpioSelect
                        name={key}
                        bind:value={mcuConfig[key]}
                        max="36"
                        disabled={mcuConfig["lan-phy"].startsWith("wt32") || mcuConfig["lan-phy"].startsWith("ox32")}
                      />
                    </td>
                  {:else}
                    <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                  {/if}
                </tr>
              {/each}
            </tbody>
          </table>
        </div>
      {/if}

      {#if mcuConfig.network !== "none"}
        {#if mcuConfigKeysWLAN.length > 0 && (mcuConfig.network === "wlan" || mcuConfig.network === "lan-wlan")}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.wlan_station")}</caption>
              <tbody>
                {#each mcuConfigKeysWLAN as key, i}
                  <tr>
                    <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                      ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                        >{mcuConfigNames[key]}</label
                      ></td
                    >
                    {#if key.endsWith("-enable")}
                      <td>
                        <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                      </td>
                    {:else}
                      <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                    {/if}
                  </tr>
                {/each}
              </tbody>
            </table>
          </div>
        {/if}

        {#if mcuConfigKeysNTP.length > 0}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.ntp_client")}</caption>
              <tbody>
                {#each mcuConfigKeysNTP as key, i}
                  <tr>
                    <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                      ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                        >{mcuConfigNames[key]}</label
                      ></td
                    >
                    {#if key.endsWith("-enable")}
                      <td>
                        <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                      </td>
                    {:else}
                      <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                    {/if}
                  </tr>
                {/each}
              </tbody>
            </table>
          </div>
        {/if}

        {#if mcuConfigKeysMQTT.length > 0}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.mqtt_client")}</caption>
              <tbody>
                {#each mcuConfigKeysMQTT as key, i}
                  <tr>
                    <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                      ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                        >{mcuConfigNames[key]}</label
                      ></td
                    >

                    {#if key.endsWith("-enable")}
                      <td>
                        <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                      </td>
                    {:else}
                      <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                    {/if}
                  </tr>
                {/each}
              </tbody>
            </table>
          </div>
        {/if}

        {#if mcuConfigKeysHTTP.length > 0}
          <div class="area">
            <table class="conf-table top_table rounded-xl overflow-hidden">
              <caption>{$_("mcuConfig.http_server")}</caption>
              <tbody>
                {#each mcuConfigKeysHTTP as key, i}
                  <tr>
                    <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                      ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                        >{mcuConfigNames[key]}</label
                      ></td
                    >

                    {#if key.endsWith("-enable")}
                      <td>
                        <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                      </td>
                    {:else}
                      <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                    {/if}
                  </tr>
                {/each}
              </tbody>
            </table>
          </div>
        {/if}
      {/if}
    {/if}
  {:else if tabIdxMcc === 1}
    <h4 class="text-center" use:tippy={{ content: $_("panes.misc.tt.header") }}>{$_("panes.misc.header")}</h4>
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
          <tbody>
            {#each mcuConfigKeysCU as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >
                {#if key.endsWith("-enable")}
                  <td>
                    <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                  </td>
                {:else}
                  <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                {/if}
              </tr>
            {/each}
          </tbody>
        </table>
        <div class="area"><button class="sb" id="id_cuasb" type="button" onclick={hClick_ScanCU}> {$_("app.wizard_cuas")} </button> <span id="id_cuasStatus"></span></div>
      </div>
    {/if}

    {#if mcuConfigKeysTime.length > 0}
      <div class="area">
        <table class="conf-table top_table rounded-xl overflow-hidden">
          <tbody>
            {#each mcuConfigKeysTime as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >
                {#if key.endsWith("-enable")}
                  <td>
                    <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                  </td>
                {:else}
                  <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                {/if}
              </tr>
            {/each}
          </tbody>
        </table>
      </div>
    {/if}

    {#if mcuConfigKeysAstro.length > 0}
      <div class="area">
        <table class="conf-table top_table rounded-xl overflow-hidden">
          <tbody>
            {#each mcuConfigKeysAstro as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >
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
          </tbody>
        </table>
      </div>
    {/if}

    <div class="area">
      <table class="conf-table top_table rounded-xl overflow-hidden">
        <caption>{$_("mcuConfig.pin_gpio")}</caption>
        <tbody>
          {#each mcuConfigKeysCc1101Pin as key, i}
            <tr>
              <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                  >{$_("mcuConfigNames.cc1101." + key)}</label
                ></td
              >
              {#if key === "rf-rx-pin"}
                <td>
                  <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} />
                  {#if $McuError_cc1101_gdo2_nc}
                    <span class="bg-red-400">(wire not connected)</span>
                  {/if}
                </td>
              {:else if key === "set-button-pin" || key === "rf-miso-pin"}
                <td>
                  <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else if key === "rf-tx-pin" || key === "rf-sclk-pin" || key === "rf-mosi-pin" || key === "rf-ss-pin"}
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
              <td use:tippy={{ content: key.startsWith("gpio") ? $_("app.cfg.mcu.user_gpio.tt.gpioxx") : $McuDocs_cliHelpConfig[key] }}
                ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                  >{mcuConfigNames[key]}</label
                ></td
              >
              {#if key.endsWith("-enable")}
                <td>
                  <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else if key === "set-button-pin"}
                <td>
                  <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} />
                  {#if mcuConfig[key] >= 0}
                    <GpioLevel name={"gpio" + mcuConfig[key]} force={true} />
                  {/if}
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
              {:else if key === "rf-tx-pin" || key === "rf-sclk-pin" || key === "rf-mosi-pin" || key === "rf-ss-pin"}
                <td>
                  <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} max="33" />
                </td>
              {:else}
                <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
              {/if}
            </tr>
          {/each}
        </tbody>
      </table>
      <div class="area">
        <label use:tippy={{ content: $_("app.cfg.mcu.user_gpio.tt.add_label") }}
          >{$_("app.cfg.mcu.user_gpio.add_label")}
          <McuConfigGpioSelect name="wizard_gpio" bind:value={wiz_gpio} show_value={false} />
          <button class="sb" type="button" disabled={wiz_gpio === -1} onclick={wiz_addGpio}>{$_("app.cfg.mcu.user_gpio.add_button")}</button>
          {@html wiz_gpio_status}
        </label>
      </div>
    </div>

    <div class="area" id="config-div">
      <table id="cfg_table_id" class="conf-table top_table rounded-xl overflow-hidden">
        <tbody>
          {#each mcuConfigKeysMisc as key, i}
            <tr>
              <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                  >{mcuConfigNames[key]}</label
                ></td
              >
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
                  {#if $McuError_cc1101_init}
                    <span class="bg-red-400">(CC1101 init failed)</span>
                  {/if}
                </td>
              {:else}
                <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
              {/if}
            </tr>
          {/each}
        </tbody>
      </table>
    </div>
  {:else if tabIdxMcc === 2}
    {#if mcuConfigKeysCC1101.length}
      <div class="area">
        <h4 class="text-center" use:tippy={{ content: $_("panes.cc1101.tt.header") }}>{$_("panes.cc1101.header")}</h4>

        <table>
          <tbody>
            {#each mcuConfigKeysCC1101 as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >
                {#if key.endsWith("-enable")}
                  <td>
                    <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                  </td>
                {:else}
                  <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                {/if}
              </tr>
            {/each}
          </tbody>
        </table>
      </div>
    {/if}
  {/if}

  <button class="sb" type="button" onclick={hClick_Reload}>{$_("app.reload")}</button>
  <button class="sb" type="button" onclick={hClick_Save}>{$_("app.save")}</button>
  <button class="sb" type="button" onclick={hClick_RestartMcu}> {$_("app.restartMcu")}</button>

  {#if tabIdxMcc === 2}
    <hr />

    <CC1101 />
  {/if}

  {#if $ReloadProgress > 0}
    <br />
    <strong>{$_("app.msg_waitForMcuRestart")}</strong>
    <br />
    <progress id="reload_progress_bar" value={$ReloadProgress} max="100"></progress>
  {/if}

  {#if tabIdxMcc === 2}
    <hr />
    <h3>Receiver Log</h3>
    <AppLog rxonly={true} />
  {/if}
</div>

<style lang="scss">
  @use "sass:color";
  @use "../styles/app.scss" as *;
  .row1 {
    background-color: color.adjust($color_bg_area, $lightness: -10%);
  }
  .row2 {
    background-color: color.adjust($color_bg_area, $lightness: 10%);
  }
  .network {
    background-color: color.adjust($color_bg_area, $lightness: -5%);
  }
</style>
