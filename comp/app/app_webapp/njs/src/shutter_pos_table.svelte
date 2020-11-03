<script>
  import * as httpFetch from "./fetch.js";
  import { Gmu } from "stores/mcu_config.js";
  import ShutterPosTableRow from "./shutter_pos_table_row.svelte";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_ALL_POS);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: gmu = $Gmu;

</script>

<style type="text/scss">

</style>

<div id="postablediv">
  <table class="top_table">
    {#each gmu as shutterCt, g}
      {#if shutterCt > 0}
        {#each { length: shutterCt+1 } as _, m}
          <ShutterPosTableRow {g} m={m} />
        {/each}
      {/if}
    {/each}
  </table>
</div>
