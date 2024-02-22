<script>
  "use strict";
  import { onMount } from "svelte";
  import * as httpFetch from "../app/fetch.js";
  import ShutterGM from "../app/shutter_gm.svelte";
  import ShutterMove from "../app/shutter_move.svelte";
  import ShutterSun from "../app/shutter_sun.svelte";
  import ShutterPct from "../app/shutter_pct.svelte";
  import { M0, GM, AutoSunEnabled, RadioCodeEnabled } from "../store/curr_shutter.js";
  import ShutterAuto from "../app/shutter_auto.svelte";
  import { GuiAcc, ShowAutomatic } from "../store/app_state";

  onMount(() => {
    setTimeout(() => {
      httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
    }, 500);
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

    {#if !$RadioCodeEnabled}
      <ShutterPct />

      <ShutterSun disabled={!$AutoSunEnabled && $M0} />
    {/if}
  </div>

  {#if $GuiAcc.shutter_auto && !$RadioCodeEnabled}
    <label class="text-center"><input type="checkbox" bind:checked={$ShowAutomatic} />Auto</label>

    {#if $ShowAutomatic}
      <div class="area">
        <ShutterAuto />
      </div>
    {/if}
  {/if}
</div>
