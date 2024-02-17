<script>
  import * as httpFetch from "../app/fetch.js";
  import { Gmu } from "../store/mcu_config.js";
  import ShutterPosTableRow from "../app/shutter_pos_table_row.svelte";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_ALL_POS);
  });

  $: gmu = $Gmu;
</script>

<div id="postablediv">
  {#each gmu as shutterCt, g}
    {#if shutterCt > 0}
      <div class="area">
        <table class="top_table">
          {#each { length: shutterCt + 1 } as _, m}
            <ShutterPosTableRow {g} {m} />
          {/each}
        </table>
      </div>
    {/if}
  {/each}
</div>
