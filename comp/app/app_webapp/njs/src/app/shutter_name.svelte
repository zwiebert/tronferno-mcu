<script>
  "use strict";
  import { _ } from "services/i18n";
  import { G, M0, Name } from "stores/curr_shutter.js";
  import * as httpFetch from "app/fetch.js";

  $: name = $Name || "";

  function shn_fromHtml_toMcu(val) {
    let tfmcu = { to: "tfmcu", shpref: { g: $G, m: $M0, "tag.NAME": val } };

    httpFetch.http_postCommand(tfmcu);
  }

  function hChange_Name() {
    shn_fromHtml_toMcu(name);
  }
</script>

<style lang="scss">

</style>

<div class="text-center">
  <input type="text" name="name" bind:value={name} on:change={hChange_Name} />
</div>
