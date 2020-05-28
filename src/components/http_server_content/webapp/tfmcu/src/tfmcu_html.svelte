<script>
  import NavTabs from "./nav_tabs.svelte";
  import ShutterBasic from "./shutter_basic.svelte";
  import ShutterAuto from "./shutter_auto.svelte";
  import ShutterAlias from "./shutter_alias.svelte";
  import ShutterPrefs from "./shutter_prefs.svelte";
  import McuConfig from "./mcu_config.svelte";
  import McuFirmware from "./mcu_firmware.svelte";
  import Testing from "./testing.svelte";
</script>

<NavTabs />

<div id="navTabs" class="container tabcontent">

  <ShutterBasic />
  <ShutterAuto />
  <ShutterAlias />
  <ShutterPrefs />
  <McuConfig />
  <McuFirmware />
  <Testing />

</div>
