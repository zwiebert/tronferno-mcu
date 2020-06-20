<script>
  "use strict";
  import { G, M, M0, GM, Pct, Name } from "./store/curr_shutter.js";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {});
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  function hClick_P() {
    onPos(document.getElementById("spi").value);
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
  <div class="flex flex-row items-center content-between">

    <button id="spb" type="button" on:click={hClick_P}>Position</button>
    <input class="w-16" id="spi" type="number" min="0" max="100" name="p" value={$Pct} />
    <input
      id="spr"
      type="range"
      min="0"
      max="100"
      name="p"
      value={$Pct}
      on:change={hChange_Pos} />

  </div>
</div>
