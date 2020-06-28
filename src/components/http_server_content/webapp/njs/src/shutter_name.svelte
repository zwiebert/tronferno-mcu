<script>
  "use strict";
  import { G, M, M0, Name } from "./store/curr_shutter.js";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";


  let on_destroy = [];
  onMount(() => {});
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: name = $Name || "";

  function shn_fromHtml_toMcu(val) {
    let tfmcu = { to: "tfmcu", shpref: { g: $G, m: $M0, "tag.NAME": val } };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
  }

  function hChange_Name() {
    shn_fromHtml_toMcu(name);
  }
</script>

<style type="text/scss">

</style>

<input
  type="text"
  name="name"
  bind:value={name}
  on:change={hChange_Name} />
