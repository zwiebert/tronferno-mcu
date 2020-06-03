<script>
   import * as httpFetch from "./fetch.js";
  import { Gmu } from "./store/mcu_config.js";
  import { Names } from "./store/shutters.js";
  import { G, M } from "./store/curr_shutter.js";
  import ShutterPosView from "./shutter_pos_view.svelte";
  import { onMount,onDestroy } from 'svelte';
  
  let on_destroy = [];
onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU|httpFetch.FETCH_ALL_POS);
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

<style>
  #pvtable, .pvgroup {
    background-color: rgb(255, 255, 255);
    border: 1px solid black;
  }
  .pvtd {
    border: 1px solid black;
  }

</style>

<div id="posviewdiv">
    {#each gmu as shutterCt, g}
      <table id="pvtable{g}" class="pvgroup"> 
      <tr>
        {#if g > 0 && shutterCt > 0}
          <th on:click={() => set_gm(g, 0)}>
    
            {$Names[g.toString() + '0'] || 'G' + g.toString()}

          </th>
          {#each { length: shutterCt } as _, m}
            <td class="pvtd" on:click={() => set_gm(g, m+1)}>
              <ShutterPosView {g} m={m + 1} />
            </td>
          {/each}
        {/if}
      </tr>
      </table>
    {/each}
</div>
