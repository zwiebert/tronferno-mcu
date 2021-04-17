<script>
  "use strict";
  import * as httpFetch from "app/fetch.js";
  import { GMH } from "stores/curr_shutter.js";
  import tippy from "sveltejs-tippy";

  $: use_motorCode = false;
  $: motorCode = "";
  $: motorCode_isValid = get_motorCode(motorCode) !== null;
  $: target = use_motorCode ? motorCode : $GMH;

  function get_motorCode() {
    const re = /^9?0[a-fA-F0-9]{4}$/;
    if (!re.test(motorCode)) return null;
    if (motorCode.startsWith("0")) return "9" + motorCode;

    return motorCode;
  }

  function postShutterCommand(cmd) {
    if (!use_motorCode) return httpFetch.http_postShutterCommand(cmd);

    const mc = get_motorCode();
    if (!mc) return;

    let req = { cmd: { a: mc, c: cmd } };
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

<hr />
<label
  >Motorcode: <input type="checkbox" bind:checked={use_motorCode} />
  <input
    class="w-24 {motorCode_isValid ? 'text-green-600' : 'text-red-600'}"
    type="text"
    maxlength="6"
    bind:value={motorCode}
    disabled={!use_motorCode}
  /></label
>

<style type="text/scss">
  @import "../styles/app.scss";
</style>
