<script>
  "use strict";
  import { G, M, M0, GM, Pct, Name } from "./store/curr_shutter.js";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";
  import DisplayIcons from "./display_icons.svelte";

  let on_destroy = [];
  onMount(() => {});
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: name = $Name || "";
  $: vm = $G ? ($M0 ? $M0 : "A") : "";
  $: vg = $G ? $G : "A";

  function shn_fromHtml_toMcu(val) {
    let tfmcu = { to: "tfmcu", shpref: { g: $G, m: $M0, "tag.NAME": val } };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
  }

  function hClick_P() {
    onPos(document.getElementById("spi").value);
  }

  function hClick_Stop() {
    httpFetch.http_postShutterCommand("stop");
  }

  function hClick_Up() {
    httpFetch.http_postShutterCommand("up");
  }

  function hClick_Down() {
    httpFetch.http_postShutterCommand("down");
  }

  function hClick_Sun() {
    httpFetch.http_postShutterCommand("sun-down");
  }

  function hChange_Name() {
    shn_fromHtml_toMcu(name);
  }

  function hChange_Pos() {
    onPos(document.getElementById("spr").value);
  }

  function onPos(pct) {
    let tfmcu = { to: "tfmcu" };
    tfmcu.send = {
      g: $G,
      m: $M,
      p: pct,
    };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
  }
</script>

<style type="text/scss">

</style>

<div id="sdi" class="inline-block">
  <div class="flex flex-row items-center">

    <button class="w-32 ft2411-udbt" type="button" on:click={hClick_Down}>
      &#x25bc;
    </button>

    <button class="m-2 ft2411-sbt" type="button" on:click={hClick_Stop}>
      STOP
    </button>

    <button class="w-32 ft2411-udbt" type="button" on:click={hClick_Up}>
      &#x25b2;
    </button>

  </div>
</div>
