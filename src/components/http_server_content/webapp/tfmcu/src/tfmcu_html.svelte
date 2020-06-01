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
  import McuFwGitTags from "./mcu_firmware_git_tags.svelte";

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

</style>

<NavTabs nav_tabs={navTabs} />

<div id="navTabs" class="container tabcontent">

  <ShutterBasic />
  <ShutterAuto />
  <ShutterAlias />
  <ShutterPrefs />
  <McuConfig />
  <McuFirmware fwbtns={fwbtns} McuFwGitTags={McuFwGitTags}/>
  {#if !isProduction}
    <Testing />
  {/if}

</div>
