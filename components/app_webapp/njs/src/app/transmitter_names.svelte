<script>
  import { run } from 'svelte/legacy';

  "use strict";
  import { _ } from "../services/i18n";
  import { G, M0, Name } from "../store/curr_shutter.js";
  import { GuiAcc } from "../store/app_state";
  import * as httpFetch from "../app/fetch.js";
  import { SelectedId, SelectedIdIsValid, TxName } from "../store/id.js";
  import tippy from "sveltejs-tippy";


  export const edit = true;

  let GMName;
  run(() => {
    GMName = $TxName || "";
  });

  run(() => {
    if ($SelectedIdIsValid) {
      const key = "TXN." + $SelectedId;
      let tfmcu = { to: "tfmcu", kvs: {} };
      tfmcu.kvs[key] = "?";

      httpFetch.http_postCommand(tfmcu);
    }
  });

  function storeName_toMcu(id, name) {
    const key = "TXN." + id;
    let tfmcu = { to: "tfmcu", kvs: {} };
    tfmcu.kvs[key] = name;

    httpFetch.http_postCommand(tfmcu);
  }

  function hChange_Name() {
    if ($SelectedIdIsValid) storeName_toMcu($SelectedId, GMName);
  }
</script>

<div class="text-center">
  <input type="text" name="name"  class="w-full" disabled={!edit || !$SelectedIdIsValid} bind:value={GMName} onchange={hChange_Name} />
</div>

<style lang="scss">
</style>
