<script>
  import { Pcts, Prefs } from "./store/shutters.js";

  export let g;
  export let m;

  const gm = g.toString() + m.toString();

  let pct = 50;

  $: pct = $Pcts[gm] || 44;
  $: name = ($Prefs["shs" + gm] || {})["tag.NAME"] || gm;

</script>

<style>
  .pvbar {
    background-color: rgb(66, 15, 15);
  }
  .pvbar-border {
    background-color: rgb(103, 155, 223);
    border: 1px solid black;
    width: 3em;
    height: 4em;
  }
</style>

<div class="pvbar-border" >
  <div class="pvbar" id="pv{gm}" style="height:{100 - pct}%" />
</div>
<br />
{name}
