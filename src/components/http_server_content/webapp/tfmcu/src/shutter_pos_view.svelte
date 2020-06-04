<script>
  import { Pcts, Names } from "./store/shutters.js";
  import {G,M,M0,GM, Pct, Name} from './store/curr_shutter.js';

  export let g;
  export let m;

  const gm = g.toString() + m.toString();

  let pct = 50;

  $: pct = $Pcts[gm] || 0;
  $: name = $Names[gm] || gm;
  $: selected = $G === 0 || ($G === g && ($M0 === m || $M0 === 0));

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
    .pvbar-sel {
  
  }
    .pvname-unsel {
    border: 4px solid white;
  }
      .pvname-sel {
    border: 4px solid rgb(145, 23, 23);
  }
</style>

<div class="pvbar-border {selected ? "pvbar-sel" : ""}" >
  <div class="pvbar" id="pv{gm}" style="height:{100 - pct}%" />
</div>
<br />
<label class="{selected ? "pvname-sel" : "pvname-unsel"}">{name}</label>
