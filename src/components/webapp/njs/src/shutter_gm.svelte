<script>
  "use strict";
  import { G, M, M0, GM, Name } from "./store/curr_shutter.js";
  import { Names } from "./store/shutters.js";
  import { Gmu } from "./store/mcu_config.js";
  import * as httpFetch from "./fetch.js";

  export let hideGroups = false;



  $: gm = $GM;
  $: nameKeys = Object.keys($Names);
  $: names = Object.values($Names);
  $: name = $Name || "";
  $: vm = $G ? ($M0 ? $M0 : "A") : "";
  $: vg = $G ? $G : "A";

  $: {
    $GM;
    httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_NAME);
  }


  function hClick_G() {
    let g = $G;
    const g_max = 7;
    for (g = g + 1; g <= 7; ++g) {
      if ($Gmu[g]) {
        break;
      }
    }
    if (g > g_max) {
      g = 0;
    }

    $G = g;
  }

  function hClick_M() {
    let m = $M + 1;
    let m_max = $Gmu[$G];
    if (m > m_max) m = 0;

    $M = m;
  }

  function hChange_Name() {
    $G = Number.parseInt(gm.substr(0, 1));
    $M = Number.parseInt(gm.substr(1, 1));
  }
</script>

<style type="text/scss">
  #sgb,
  #smb {
    width: 3rem;
    height: 2rem;
  }
</style>

<div class="text-center">
  <!--
  <button id="sgb" type="button" on:click={hClick_G}>G{vg}</button>
  <button id="smb" disabled="{vm === ''}" type="button" on:click={hClick_M}>E{vm}</button>
-->
  <select
    id="smn"
    class="text-center"
    bind:value={gm}
    on:change={hChange_Name}
    on:blur={hChange_Name}>

    <option value="00" disabled={hideGroups}>A</option>
    {#each [1, 2, 3, 4, 5, 6, 7] as g}
      {#if $Gmu[g]}
        {#each { length: $Gmu[g] + 1 } as _, m}
          <option
            value={g.toString() + m.toString()}
            disabled={m === 0 && hideGroups}>
            {g}{m === 0 ? 'A' : m} {$Names[g.toString() + m.toString()] || ''}
          </option>
        {/each}
      {/if}
    {/each}
  </select>

</div>
