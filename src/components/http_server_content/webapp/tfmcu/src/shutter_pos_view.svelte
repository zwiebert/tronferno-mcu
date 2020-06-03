<script>
  import { Pcts, Names } from "./store/shutters.js";

  export let g;
  export let m;

  const gm = g.toString() + m.toString();

  let pct = 50;

  $: pct = $Pcts[gm] || 0;
  $: name = $Names[gm] || gm;

</script>

<style>
  .pvbar {
    background-color: #4e4a22;
  }
  .pvbar-border {
    background-color: rgb(103, 155, 223);
    border: 1px solid black;
    width: 100%;
    height: 4em;
  }
</style>

<div class="pvbar-border" >
  <div class="pvbar" id="pv{gm}" style="height:{100 - pct}%" />
</div>
<br />
{name}
