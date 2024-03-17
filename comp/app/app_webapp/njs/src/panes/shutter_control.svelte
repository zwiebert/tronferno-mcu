<script>
  "use strict";
  import { _ } from "../services/i18n";
  import { onMount } from "svelte";
  import * as httpFetch from "../app/fetch.js";
  import ShutterGM from "../app/shutter_gm.svelte";
  import ShutterMove from "../app/shutter_move.svelte";
  import ShutterSun from "../app/shutter_sun.svelte";
  import ShutterPct from "../app/shutter_pct.svelte";
  import { M0, GM, G, AutoSunEnabled, RadioCodeEnabled } from "../store/curr_shutter.js";
  import ShutterAuto from "../app/shutter_auto.svelte";
  import { GuiAcc, ShowAutomatic, ShowPositions } from "../store/app_state";
  import ShutterPosTable from "../app/shutter_pos_table.svelte";

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

  {#if !$RadioCodeEnabled}
    <label class="text-center"><input type="checkbox" bind:checked={$ShowPositions} />{$_("navTab.main.percent")}</label>
    {#if $GuiAcc.shutter_auto}
      <label class="text-center"><input type="checkbox" bind:checked={$ShowAutomatic} />{$_("navTab.main.auto")}</label>
    {/if}
    {#if $ShowPositions}
      <ShutterPosTable show_group={$G} />
    {/if}
    {#if $GuiAcc.shutter_auto && $ShowAutomatic}
      <div class="area">
        <ShutterAuto />
      </div>
    {/if}
  {/if}
</div>
