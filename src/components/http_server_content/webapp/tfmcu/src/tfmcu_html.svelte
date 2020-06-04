<script>
  import NavTabs from "./nav_tabs.svelte";
  import ShutterBasic from "./shutter_basic.svelte";
  import ShutterGM from "./shutter_gm.svelte";
  import ShutterAuto from "./shutter_auto.svelte";
  import ShutterAlias from "./shutter_alias.svelte";
  import ShutterPrefs from "./shutter_prefs.svelte";
  import ShutterPosViews from "./shutter_pos_views.svelte";
  import McuConfig from "./mcu_config.svelte";
  import McuFirmware from "./mcu_firmware.svelte";
  import Testing from "./testing.svelte";
  import * as httpFetch from "./fetch.js";
  import McuFwGitTags from "./mcu_firmware_git_tags.svelte";
  import { TabIdx } from "./store/app_state.js";

  export let isProduction = false;

  $: tabIdx = $TabIdx;

  let navTabs = [
    { text: "Command" },
    { text: "Automatic" },
    { text: "Config" },
    { text: "Positions" },
    { text: "Firmware" },
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

<style>
  .container {
    border: 1px solid #ccc;
    background-color: #eee;
    font-size: 270%;
  }

  @media only screen and (max-resolution: 96dpi) {
    .container {
      font-size: 200%;
    }
  }

  @media (min-width: 1000px) {
    .container {
      font-size: 100%;
    }
  }
  .tabcontent {
    padding: 6px 12px;
    border: 1px solid #ccc;
    border-top: none;
  }
</style>

<NavTabs nav_tabs={navTabs} />

<div id="navTabs" class="container tabcontent">
  {#if tabIdx === 0}
    <ShutterBasic />
    <ShutterPosViews />
  {:else if tabIdx === 1}
    <ShutterGM
      gmc_fetch_mask={httpFetch.FETCH_AUTO | httpFetch.FETCH_SHUTTER_NAME} />
    <hr />
    <ShutterAuto />
  {:else if tabIdx === 2}
    <McuConfig />
  {:else if tabIdx === 3}
    <ShutterGM
      gmc_fetch_mask={httpFetch.FETCH_ALIASES | httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME} />
    <hr />
    <ShutterAlias />
    <hr />
    <ShutterPrefs />
  {:else if tabIdx === 4}
    <McuFirmware {fwbtns} {McuFwGitTags} />
  {:else if tabIdx === 5}
    <Testing />
  {/if}
</div>
