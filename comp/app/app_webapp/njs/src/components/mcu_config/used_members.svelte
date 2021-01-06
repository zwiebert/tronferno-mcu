<script>
  import { Gmu } from "stores/mcu_config.js";

  export let caption;
  export let name;
  export let value;
  export let changed;


  let gmu = [...$Gmu];

  $: {
    update_value(gmu);
  }

  $: {
    update_gmu($Gmu);
  }


  function update_gmu(arr) {
    gmu = [...arr];
  } 

  function update_value(gmu) {
    let val = "";

    let written = false; // to skip leading zeros
    for (let i = 7; i >= 1; --i) {
      let mct = gmu[i];
      if (mct !== "0" || written) {
        val += mct;
        written = true;
      }
    }
    let x = value;
    val += x[x.length - 1]; // push back unused lowest nibble

    value = val;
  }



</script>

<table id="gmu-table" class="border-black border-2">
  <caption  class="{changed ? 'font-bold' : 'font-normal'}">{caption}</caption>
  <tr>
    {#each { length: 7 } as _, i}
      <td><label>G{i + 1}</label></td>
    {/each}
  </tr>
  <tr>
    {#each gmu as n, i}
      {#if i > 0}
        <td><input bind:value={gmu[i]} id="gmu{i}" style="width:2em;" type="number" min="0" max="7" /></td>
      {/if}
    {/each}
  </tr>
</table>
