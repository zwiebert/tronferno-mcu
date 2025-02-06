<script>
  ("use strict");
  import { onMount } from "svelte";
  import { _ } from "../services/i18n";
  import { G, M0, Name } from "../store/curr_shutter.js";
  import { GuiAcc } from "../store/app_state";
  import * as httpFetch from "../app/fetch.js";
  import { SelectedId, SelectedIdIsValid, TxName, is_valid_fernotron_id } from "../store/id.js";
  import tippy from "sveltejs-tippy";

  export const edit = true;

  onMount(() => {
    fetch_txName($SelectedId);
  });

  let GMName = $state($TxName || "");
  $effect(() => {
    GMName = $TxName || "";
  });

  $effect(() => {
    fetch_txName($SelectedId);
  });

  function fetch_txName(id) {
    if (is_valid_fernotron_id(id)) {
      const key = "TXN." + id;
      let tfmcu = { to: "tfmcu", kvs: {} };
      tfmcu.kvs[key] = "?";
      httpFetch.http_postCommand(tfmcu);
    }
  }

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
  <input type="text" name="name" class="w-full" disabled={!edit || !$SelectedIdIsValid} bind:value={GMName} onchange={hChange_Name} />
</div>

<style lang="scss">
</style>
