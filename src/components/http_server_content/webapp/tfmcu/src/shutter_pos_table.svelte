<script>
  import * as httpFetch from "./fetch.js";
  import { Gmu } from "./store/mcu_config.js";
  import { Names, Pcts } from "./store/shutters.js";
  import { G, M } from "./store/curr_shutter.js";
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

  function set_gm(g, m) {
    G.set(g);
    M.set(m);
  }
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
