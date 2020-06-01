<script>
  import NavTabs from "./nav_tabs.svelte";
  import ShutterBasic from "./shutter_basic.svelte";
  import ShutterAuto from "./shutter_auto.svelte";
  import ShutterAlias from "./shutter_alias.svelte";
  import ShutterPrefs from "./shutter_prefs.svelte";
  import McuConfig from "./mcu_config.svelte";
  import McuFirmware from "./mcu_firmware.svelte";
  import Testing from "./testing.svelte";
  import * as httpFetch from "./fetch.js";

  export let isProduction = false;

  let navTabs = [
    {
      text: "Command",
      div_id: ["senddiv"],
      fetch_gm: httpFetch.FETCH_POS | httpFetch.FETCH_SHUTTER_NAME,
    },
    {
      text: "Automatic",
      div_id: ["senddiv", "autodiv"],
      fetch_gm:
        httpFetch.FETCH_AUTO |
        httpFetch.FETCH_POS |
        httpFetch.FETCH_SHUTTER_NAME,
    },
    { text: "Config", div_id: ["configdiv"], fetch: httpFetch.FETCH_CONFIG },
    {
      text: "Positions",
      div_id: ["senddiv", "aliasdiv", "shprefdiv"],
      fetch: httpFetch.FETCH_ALIASES,
      fetch_gm:
        httpFetch.FETCH_POS |
        httpFetch.FETCH_SHUTTER_PREFS |
        httpFetch.FETCH_SHUTTER_NAME,
    },
    {
      text: "Firmware",
      div_id: ["id-fwDiv"],
      fetch_init: httpFetch.FETCH_VERSION | httpFetch.FETCH_GIT_TAGS,
    },
  ];

 
    // eslint-disable-next-line no-unused-labels
    testing: if (!isProduction) {
       navTabs = navTabs.concat([{ text: "Tests", div_id: ["testsdiv"] }]);
    }
  
</script>

<style>

</style>

<NavTabs nav_tabs={navTabs} />

<div id="navTabs" class="container tabcontent">

  <ShutterBasic />
  <ShutterAuto />
  <ShutterAlias />
  <ShutterPrefs />
  <McuConfig />
  <McuFirmware />
  {#if !isProduction}
  <Testing />
  {/if}

</div>
