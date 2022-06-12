<script>
  "use strict";
  import * as httpFetch from "app/fetch.js";
  import { GMH, RadioCode, RadioCodeEnabled, Address } from "stores/curr_shutter.js";
  import tippy from "sveltejs-tippy";
  import SelectRadioCode from "components/select_radio_code.svelte";

  $: target = $RadioCodeEnabled ? $RadioCode : $GMH;


  function postShutterCommand(cmd) {
    if (!$RadioCodeEnabled) return httpFetch.http_postShutterCommand(cmd);

    const mc = $RadioCode;
    if (!mc) return;

    let req = { cmd: { ...$Address, c: cmd } };
    httpFetch.http_postCommand(req);
  }
  function hClick_RotDir() {
    postShutterCommand("rot-dir");
  }

  function hClick_Up() {
    postShutterCommand("up");
    setTimeout(() => {
      postShutterCommand("stop");
    }, 1000);
  }

  function hClick_Down() {
    postShutterCommand("down");
    setTimeout(() => {
      postShutterCommand("stop");
    }, 1000);
  }
</script>

<div class="inline-block">
  <div class="flex flex-row items-center">
    <button class="w-16 h-16 text-lg" type="button" on:click={hClick_Down}> Test &#x25bc; </button>

    <button class="m-2 w-32 h-16 text-lg" type="button" on:click={hClick_RotDir}> Toggle ({target}) </button>

    <button class="w-16 h-16 text-lg" type="button" on:click={hClick_Up}> Test &#x25b2; </button>
  </div>
</div>

<style lang="scss">
  @import "../styles/app.scss";
</style>
