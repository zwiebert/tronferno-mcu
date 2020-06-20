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

  function hClick_Sun() {
    httpFetch.http_postShutterCommand("sun-down");
  }

  function hClick_SunPos() {
    httpFetch.http_postShutterCommand("sun-inst");
  }
</script>

<style type="text/scss">

</style>

<div id="sdi" class="inline-block">
  <div class="flex flex-row items-center content-between">

    <button id="sspb" class="sb" type="button" on:click={hClick_Sun}>
      Sun &#x25bc;
    </button>

    <button id="sspb" class="sb" type="button" on:click={hClick_SunPos}>
      INST
    </button>

  </div>
</div>
