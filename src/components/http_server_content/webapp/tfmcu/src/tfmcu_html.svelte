<script>
  import NavTabs from "./nav_tabs.svelte";
  import ShutterBasic from "./shutter_basic.svelte";
  import ShutterGM from "./shutter_gm.svelte";
  import ShutterAuto from "./shutter_auto.svelte";
  import ShutterAlias from "./shutter_alias.svelte";
  import ShutterPrefs from "./shutter_prefs.svelte";
  import ShutterPosViews from "./shutter_pos_views.svelte";
  import ShutterPosTable from "./shutter_pos_table.svelte";
  import McuConfig from "./mcu_config.svelte";
  import McuFirmware from "./mcu_firmware.svelte";
  import Testing from "./testing.svelte";
  import * as httpFetch from "./fetch.js";
  import McuFwGitTags from "./mcu_firmware_git_tags.svelte";
  import { TabIdx } from "./store/app_state.js";
  import Layout from "./layout.svelte";

  export let isProduction = false;

  $: tabIdxMain = $TabIdx["main"];
  $: tabIdxSettings = $TabIdx["settings"];
  $: visibleMcuConfig = false;
  $: visiblePairedControllers = false;
  $: visibleMovementDurations = false;

  let navTabs = [
    { text: "Move" },
    { text: "Pct" },
    { text: "Auto" },
    { text: "Settings" },
    { text: "Firmware" },
  ];

  let navTabsSettings = [
    { text: "MCU" },
    { text: "Aliases" },
    { text: "Durations" },
  ];

  let fwbtns = [
    { name: "latest master firmware", ota_name: "github-master" },
    { name: "latest beta firmware", ota_name: "github-beta" },
  ];

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    navTabs.push({ text: "Tests", div_id: ["testsdiv"] });
    fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:3000/tronferno-mcu.bin",
    });
  }
</script>

<style type="text/scss">
  .container {
    border: 1px solid #ccc;
    background-color: #eee;
    font-size: 100%;
  }

  .tabcontent {
    padding: 6px 12px;
    border: 1px solid #ccc;
    border-top: none;
  }
</style>

<Layout />

<NavTabs nav_tabs={navTabs} name="main" />

<div id="navTabs" class="container tabcontent">
  {#if tabIdxMain === 0}
    <ShutterBasic />
  {:else if tabIdxMain === 1}
    <br />
    <ShutterPosTable />
  {:else if tabIdxMain === 2}
    <ShutterGM
      gmc_fetch_mask={httpFetch.FETCH_AUTO | httpFetch.FETCH_SHUTTER_NAME} />
    <br />
    <ShutterAuto />
  {:else if tabIdxMain === 3}
    <NavTabs nav_tabs={navTabsSettings} name="settings" />
    {#if tabIdxSettings === 0}
      <McuConfig />
    {/if}

    {#if tabIdxSettings === 1}
      <ShutterGM
        gmc_fetch_mask={httpFetch.FETCH_ALIASES | httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME} />
      <br />
      <ShutterAlias />
    {/if}

    {#if tabIdxSettings === 2}
      <ShutterGM
        gmc_fetch_mask={httpFetch.FETCH_ALIASES | httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME} />
      <br />
      <ShutterPrefs />
    {/if}
  {:else if tabIdxMain === 4}
    <McuFirmware {fwbtns} {McuFwGitTags} />
  {:else if tabIdxMain === 5}
    <Testing />
  {/if}
</div>
