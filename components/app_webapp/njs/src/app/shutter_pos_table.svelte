<script>
  import * as httpFetch from "../app/fetch.js";
  import { Gmu } from "../store/mcu_config.js";
  import ShutterPosTableRow from "../app/shutter_pos_table_row.svelte";
  import { onMount } from "svelte";

  /**
   * @typedef {Object} Props
   * @property {number} [show_group]
   */

  /** @type {Props} */
  let { show_group = 0 } = $props();

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_ALL_POS);
  });

  let gmu = $derived($Gmu);
</script>

<div id="postablediv">
  {#each gmu as shutterCt, g}
    {#if shutterCt > 0 && (show_group === 0 || show_group === g)}
      <div class="area">
        <table class="top_table w-full">
          {#each { length: shutterCt + 1 } as _, m}
            <ShutterPosTableRow {g} {m} />
          {/each}
        </table>
      </div>
    {/if}
  {/each}
</div>
