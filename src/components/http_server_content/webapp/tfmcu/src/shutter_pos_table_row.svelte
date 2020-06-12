<script>
  import { Pcts, Names } from "./store/shutters.js";
  import { G, M0, M, GM} from "./store/curr_shutter.js";
  import * as httpFetch from "./fetch.js";

  export let g;
  export let m;

  const gm = g.toString() + m.toString();

  $: pct = $Pcts[gm] || 0;
  $: name = $Names[gm] || gm;
  $: selected = $G === 0 || ($G === g && ($M0 === m || $M0 === 0));

  let sliderElement;

  function onPos(g, m, pct) {
    let tfmcu = { to: "tfmcu" };
    tfmcu.send = {
      g: g,
      m: m,
      p: pct,
    };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
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

th {
  text-align: left;
}

</style>

<tr class="{$GM === gm ? "is-active":""}">
  <th on:click={set_gm}>{name}</th>
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
