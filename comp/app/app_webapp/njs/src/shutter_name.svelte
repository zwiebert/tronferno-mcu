<script>
  "use strict";
  import { _ } from "services/i18n";
  import { G, M0, Name } from "stores/curr_shutter.js";
  import * as httpFetch from "./fetch.js";

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

<div class="text-center">
  <label>{$_('app.msg_enterNewName')}</label>
  <br />
  <input type="text" name="name" bind:value={name} on:change={hChange_Name} />
</div>
