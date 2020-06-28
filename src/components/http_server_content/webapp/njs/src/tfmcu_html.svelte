<script>
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
  import McuFirmware from "./mcu_firmware.svelte";
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

  let navTabs = ["Move", "Pct", "Auto", "Settings", "Firmware"];
  let navTabsSettings = ["MCU", "Aliases", "Durations", "Name", "App"];

  let fwbtns = [
    { name: "latest master firmware", ota_name: "github-master" },
    { name: "latest beta firmware", ota_name: "github-beta" },
  ];

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    navTabs.push("Tests");
    fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:3000/tronferno-mcu.bin",
    });
  }
</script>

<style type="text/scss">
  @import "./styles/app.scss";

  .area {
    @apply mt-4 p-2 bg-area;
  }
</style>

<Layout />

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <NavTabs nav_tabs={navTabs} name="main" />

  {#if tabIdxMain === 0}
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
    <NavTabs nav_tabs={navTabsSettings} name="settings" />
    {#if tabIdxSettings === 0}
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
      <McuFirmware {fwbtns} {McuFwGitTags} />
    </div>
  {:else if tabIdxMain === 5}
    <Testing />
  {/if}
</div>
