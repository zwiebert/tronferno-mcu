<script>
  "use strict";
  import { onMount } from "svelte";
  import * as httpFetch from "app/fetch.js";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterMove from "app/shutter_move.svelte";
  import ShutterSun from "app/shutter_sun.svelte";
  import ShutterPct from "app/shutter_pct.svelte";
  import { M0, GM, AutoSunEnabled, RadioCodeEnabled } from "stores/curr_shutter.js";


  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
  });

  $: {
    $GM;
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
  }


</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM />
  </div>

  <div class="area">
    <ShutterMove />
  </div>

  {#if !$RadioCodeEnabled}
  <div class="area">
    <ShutterPct />
  </div>
  {/if}

  {#if $AutoSunEnabled || !$M0 || $RadioCodeEnabled}
    <div class="area">
      <ShutterSun />
    </div>
  {/if}
</div>
