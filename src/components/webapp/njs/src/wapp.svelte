<script>
  import { _ } from './services/i18n';
  import NavTabs from "./nav_tabs.svelte";
  import ShutterMove from "./shutter_move.svelte";
  import ShutterGM from "./shutter_gm.svelte";
  import ShutterAuto from "./shutter_auto.svelte";
  import ShutterAlias from "./shutter_alias.svelte";
  import ShutterPrefs from "./shutter_prefs.svelte";
  import PrefApp from "./pref_app.svelte";
  import ShutterSun from "./shutter_sun.svelte";
  import ShutterPct from "./shutter_pct.svelte";
  import ShutterName from "./shutter_name.svelte";
  import ShutterPosTable from "./shutter_pos_table.svelte";
  import McuConfig from "./mcu_config.svelte";
  import McuFirmwareUpd from "./mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "./mcu_firmware_info.svelte";
  import Testing from "./testing.svelte";
  import * as httpFetch from "./fetch.js";
  import McuFwGitTags from "./mcu_firmware_git_tags.svelte";
  import { TabIdx } from "./store/app_state.js";
  import Layout from "./layout.svelte";

  export let isProduction = false;

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
  $: visibleMcuConfig = false;
  $: visiblePairedControllers = false;
  $: visibleMovementDurations = false;

  let fwbtns = [
    { name: "latest master firmware", ota_name: "github-master" },
    { name: "latest beta firmware", ota_name: "github-beta" },
  ];

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:3000/tronferno-mcu.bin",
    });
  }
</script>

<style type="text/scss">

</style>

<Layout />

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
  <NavTabs nav_tabs={[$_('app.nav_main_move'), $_('app.nav_main_percent'),$_('app.nav_main_auto'),
   $_('app.nav_main_config'), $_('app.nav_main_firmware'), ...isProduction ? [] : ["Test"]]} name="main" />
  </div>
  {#if !tabIdxMain}
    <div class="mt-4 p-2">
      <ShutterGM
        gmc_fetch_mask={httpFetch.FETCH_AUTO | httpFetch.FETCH_POS | httpFetch.FETCH_SHUTTER_NAME} />
    </div>

    <div class="area">
      <ShutterMove />
    </div>

    <div class="area">
      <ShutterPct />
    </div>

    <div class="area">
      <ShutterSun />
    </div>
  {:else if tabIdxMain === 1}
    <div class="area">
      <ShutterPosTable />
    </div>
  {:else if tabIdxMain === 2}
    <div class="mt-4 p-2">
      <ShutterGM
        gmc_fetch_mask={httpFetch.FETCH_AUTO | httpFetch.FETCH_SHUTTER_NAME} />
    </div>
    <div class="area">
      <ShutterAuto />
    </div>
  {:else if tabIdxMain === 3}
  <div class="navtab-sub">
    <NavTabs nav_tabs={[$_('app.nav_cfg_mcu'), $_('app.nav_cfg_aliases'), 
    $_('app.nav_cfg_durations'), $_('app.nav_cfg_name'), $_('app.nav_cfg_app')]} name="settings" />
  </div>
    {#if !tabIdxSettings}
      <div class="area">
        <McuConfig />
      </div>
    {:else if tabIdxSettings === 1}
      <div class="mt-4 p-2">
        <ShutterGM
          gmc_fetch_mask={httpFetch.FETCH_ALIASES | httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME} />
      </div>
      <div class="area">
        <ShutterAlias />
      </div>
    {:else if tabIdxSettings === 2}
      <div class="mt-4 p-2">
        <ShutterGM
          gmc_fetch_mask={httpFetch.FETCH_ALIASES | httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME} />
      </div>
      <div class="area">
        <ShutterPrefs />
      </div>
    {:else if tabIdxSettings === 3}
      <div class="mt-4 p-2">
        <ShutterGM gmc_fetch_mask={httpFetch.FETCH_SHUTTER_NAME} />
      </div>
      <div class="area">
        <ShutterName />
      </div>
    {:else if tabIdxSettings === 4}
      <div class="area">
        <PrefApp />
      </div>
    {/if}
  {:else if tabIdxMain === 4}
    <div class="area">
     
        <h4>NetMCU ESP32</h4>
        <McuFirmwareUpd {fwbtns}  {McuFwGitTags} chip="" updSecs="15" />
        <McuFirmwareInfo />
     
    </div>
  {:else if tabIdxMain === 5}
    <Testing />
  {/if}
</div>
