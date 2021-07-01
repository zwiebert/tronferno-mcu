<script>
  import { Pcts, Names } from "stores/shutters.js";
  import { G, M0, M, GM } from "stores/curr_shutter.js";
  import * as httpFetch from "app/fetch.js";

  export let g;
  export let m;

  const gm = g.toString() + m.toString();

  $: pct = $Pcts[gm] || 0;
  $: name = gm + ($Names[gm] ? " " + $Names[gm] : "");
  $: selected = $G === 0 || ($G === g && ($M0 === m || $M0 === 0));

  let sliderElement;

  function onPos(g, m, pct) {
    let tfmcu = { to: "tfmcu" };
    tfmcu.send = {
      g: g,
      m: m,
      p: pct,
    };

    httpFetch.http_postCommand(tfmcu);
  }

  function hChange_Pos() {
    set_gm();
    onPos(g, m, sliderElement.value);
  }

  function set_gm() {
    G.set(g);
    M.set(m);
  }
</script>

<style type="text/scss">

</style>

<tr class={$GM === gm ? 'is-selected' : ''}>
  <th class="text-left" on:click={set_gm}>{name}</th>
  <td>{pct}%</td>
  <td>
    <input
      bind:this={sliderElement}
      type="range"
      min="0"
      max="100"
      value={pct}
      on:change={hChange_Pos} />
  </td>
</tr>
