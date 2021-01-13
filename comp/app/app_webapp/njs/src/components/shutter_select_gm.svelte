<script>
  "use strict";
  import { G, M, GM } from "stores/curr_shutter.js";
  import { Names } from "stores/shutters.js";
  import { Gmu } from "stores/mcu_config.js";
  import * as httpFetch from "app/fetch.js";

  export let hideGroups = false;

  $: gm = $GM;
  $: names = Object.values($Names);
  $: a = "A " + $Names["00"] || "";

  $: {
    $G = Number.parseInt(gm.substr(0, 1));
    $M = Number.parseInt(gm.substr(1, 1));
    httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_NAME);
  }
</script>

<select bind:value={gm}>
  <option value="00" disabled={hideGroups}>{a}</option>
  {#each [1, 2, 3, 4, 5, 6, 7] as g}
    {#if $Gmu[g]}
      {#each { length: $Gmu[g] + 1 } as _, m}
        <option
          value={g.toString() + m.toString()}
          disabled={m === 0 && hideGroups}>
          {g}{m || 'A'}
          {$Names[g.toString() + m.toString()] || ''}
        </option>
      {/each}
    {/if}
  {/each}
</select>
