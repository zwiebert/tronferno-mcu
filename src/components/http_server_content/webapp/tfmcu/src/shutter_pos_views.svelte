<script context="module">
  import * as httpFetch from "./fetch.js";
  export const fetch_init = httpFetch.FETCH_GMU;
  export const div = "posviewdiv";
</script>

<script>
  import { Gmu } from "./store/mcu_config.js";
  import { Prefs } from "./store/shutters.js";
  import { G, M } from "./store/curr_shutter.js";
  import ShutterPosView from "./shutter_pos_view.svelte";

  $: gmu = $Gmu;
  $: prefs = $Prefs;

  function set_gm(g, m) {
    G.set(g);
    M.set(m);
  }
</script>

<style>
  #pvtable {
    background-color: rgb(255, 255, 255);
    border: 1px solid black;
  }
  .pvtd {
    border: 1px solid black;
  }
    .pvgr {
  background-color: rgb(139, 107, 107);
  width: 100%;
  height: 100%;
  }
</style>

<div id="posviewdiv">
  <table id="pvtable">
    {#each gmu as shutterCt, g}
      <tr>
        {#if g > 0 && shutterCt > 0}
          <th on:click={() => set_gm(g, 0)}>
    
            {(prefs['shs' + g.toString() + '0'] || {})['tag.NAME'] || 'G' + g.toString()}

          </th>
          {#each { length: shutterCt } as _, m}
            <td class="pvtd" on:click={() => set_gm(g, m+1)}>
              <ShutterPosView {g} m={m + 1} />
            </td>
          {/each}
        {/if}
      </tr>
    {/each}
  </table>
</div>
