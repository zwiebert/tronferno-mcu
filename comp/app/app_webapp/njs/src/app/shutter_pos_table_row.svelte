<script>
  import { Pcts, Names } from "../store/shutters.js";
  import { G, M0, M, GM } from "../store/curr_shutter.js";
  import * as httpFetch from "../app/fetch.js";

  export let g;
  export let m;

  const gm = g.toString() + m.toString();

  $: pct = $Pcts[gm] || 0;
  $: ge = g.toString() + (m ? m.toString() : "A");
  $: name = $Names[gm] ? " " + $Names[gm] : "";
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

<tr class="align-middle" on:click={set_gm}>
  <th class="{selected ? 'bg-area-selected' : ''}">{ge}</th>
  <td class="text-left w-full">{name}</td>

  <td class="text-right " style="width:4ch">{pct}</td>
  <td class="text-right">
    <input
      class="align-middle"
      bind:this={sliderElement}
      type="range"
      min="0"
      max="100"
      value={pct}
      on:change={hChange_Pos}
      on:input={(evt) => {
        pct = evt.target.value;
      }}
    />
  </td>
</tr>

<style lang="scss">
</style>
