<script>
  "use strict";
  import { G, M, Pct } from "../store/curr_shutter.js";
  import * as httpFetch from "../app/fetch.js";
  import { onMount, onDestroy } from "svelte";

  let pct = $state($Pct || 0);
  $effect(() => {
    pct = $Pct || 0;
  });

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALL_POS); // XXX: Need only one position to fetch
  });

  function onPos(pct) {
    let tfmcu = { to: "tfmcu" };
    tfmcu.send = {
      g: $G,
      m: $M,
      p: pct,
    };

    httpFetch.http_postCommand(tfmcu);
  }
</script>

<div class="flex w-full">
  <span class="text-right inline align-middle" style="width:4ch;">{pct}</span>
  <input
    class="w-full inline align-middle"
    type="range"
    min="0"
    max="100"
    value={pct}
    onchange={(evt) => {
      onPos(evt.target.value);
    }}
    oninput={(evt) => {
      pct = evt.target.value;
    }}
  />
</div>

