<script>
  import { Pcts, Names } from "./store/shutters.js";
  import {G,M0 } from './store/curr_shutter.js';

  export let g;
  export let m;

  const gm = g.toString() + m.toString();

  let pct = 50;

  $: pct = $Pcts[gm] || 0;
  $: name = $Names[gm] || gm;
  $: selected = $G === 0 || ($G === g && ($M0 === m || $M0 === 0));

</script>

<style type="text/scss">
 
  .pvbar {
    background-color: #111730;
  }
  .pvbar-border {
    background-color: rgb(103, 155, 223);
    border: 1px solid black;
    width: 100%;
    height: 4em;
  }

    .pvname-unsel {
    border: 4px solid white;
  }
      .pvname-sel {
    border: 4px solid rgb(66, 23, 145);
  }
</style>

<div class="pvbar-border {selected ? "pvbar-sel" : ""}" >
  <div class="pvbar" id="pv{gm}" style="height:{100 - pct}%" />
</div>
<br />
<label class="{selected ? "pvname-sel" : "pvname-unsel"}">{name}</label>
