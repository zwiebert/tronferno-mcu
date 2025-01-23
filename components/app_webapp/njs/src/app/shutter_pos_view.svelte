<script>
  import { Pcts, Names } from "../store/shutters.js";
  import { G, M0 } from "../store/curr_shutter.js";

  let { g, m } = $props();

  const gm = g.toString() + m.toString();

  let pct = $state($Pcts[gm] || 0);

  $effect(() => {
    pct = $Pcts[gm] || 0;
  });
  let name = $derived(gm + ($Names[gm] ? " " + $Names[gm] : ""));
  let selected = $derived($G === 0 || ($G === g && ($M0 === m || $M0 === 0)));
</script>

<div class="pvbar-border {selected ? 'pvbar-sel' : ''}">
  <div class="pvbar" id="pv{gm}" style="height:{100 - pct}%"></div>
</div>
<br />
<span class={selected ? "pvname-sel" : "pvname-unsel"}>{name}</span>

<style lang="scss">
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
