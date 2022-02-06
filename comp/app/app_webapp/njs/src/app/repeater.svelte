<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GM, GMH } from "stores/curr_shutter.js";
  import { SelectedId } from "stores/id.js";
  import { Gmu, GmuMaxM, McuConfig } from "stores/mcu_config.js";
  import { Aliases } from "stores/shutters.js";
  import { ShowHelp } from "stores/app_state.js";
  import { Pras, ReceivedAddresses } from "stores/alias.js";
  import { onMount, onDestroy } from "svelte";
  import tippy from "sveltejs-tippy";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_REPEATER_IDS);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: mcuConfig = $McuConfig;
  $: RepeaterIDs = new Set("rf-repeater" in mcuConfig ? $McuConfig["rf-repeater"].match(/.{6}/g) : []);
  $: selectedId_isValid = id_isValid($SelectedId);

  $: selectedRepId = "";

  function id_isValid(id) {
    const re = /^[12]0[0-9A-Fa-f]{4}$/;
    return re.test(id);
  }

  function add_repeaterID(id) {
    httpFetch.http_postCommand({ config: { "rf-repeater": "+" + id } });
    
/*
    RepeaterIDs.update((obj) => {
      obj.add(id);
      return obj;
    });
    */
  }

  function remove_repeaterID(id) {
    httpFetch.http_postCommand({ config: { "rf-repeater": "-" + id } });
    /*
    RepeaterIDs.update((obj) => {
      obj.delete(id);
      return obj;
    });
    */
  }
</script>

<div id="aliasdiv text-center">
  <br />

  <div class="area text-center">
    <h5>Sender-IDs to be repeated</h5>
    <select id="repeaterIDs" size="5" bind:value={selectedRepId}>
      {#each [...RepeaterIDs].sort() as key}
        <option>{key}</option>
      {/each}
    </select>
    <br />
    <button
      type="button"
      disabled={!selectedId_isValid || RepeaterIDs.has($SelectedId)}
      on:click={() => {
        add_repeaterID($SelectedId);
      }}>Add {$SelectedId}</button
    >

    <button
      type="button"
      disabled={!RepeaterIDs.has(selectedRepId)}
      on:click={() => {
        remove_repeaterID(selectedRepId);
      }}>Remove {RepeaterIDs.has(selectedRepId) ? selectedRepId : ""}</button
    >
  </div>
</div>

<style type="text/scss">
  @import "../styles/app.scss";

  table,
  td,
  tr {
    border-color: $color_border_main_area;
    border-style: solid;
    border-width: 1px;
    border-radius: 0.75rem;
    overflow: hidden;
    border-collapse: collapse;
    margin: 0rem;
    padding: 0rem 0.25rem;
    border-gap: 0;
  }
</style>
