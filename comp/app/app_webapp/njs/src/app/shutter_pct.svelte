<script>
  "use strict";
  import { G, M, Pct } from "../store/curr_shutter.js";
  import * as httpFetch from "../app/fetch.js";
  import { onMount, onDestroy } from "svelte";

  $: pct = $Pct != null ? $Pct : 0;

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
    on:change={(evt) => {
      onPos(evt.target.value);
    }}
    on:input={(evt) => {
      pct = evt.target.value;
    }}
  />
</div>

