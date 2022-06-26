<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { SelectedId, TxNames } from "stores/id.js";
  import { McuConfig } from "stores/mcu_config.js";
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
  function rxOptTxt(id) {
    return (id.startsWith("20") ? "\u263C " : "\u2195 ") + id + " " + ($TxNames[id] || "");
  }
</script>

<div id="aliasdiv text-center">
  <br />

  <div class="area text-center">
    <h5  use:tippy={{ content: $_("app.repeater.ids.tt.header") }}>{$_("app.repeater.ids.header")}</h5>
    <select id="repeaterIDs" size="5" bind:value={selectedRepId}>
      {#each [...RepeaterIDs].sort() as key}
        <option value={key}>{rxOptTxt(key)}</option>
      {/each}
    </select>
    <br />
    <button
    use:tippy={{ content: $_("app.repeater.ids.tt.add_button") }}
      type="button"
      disabled={!selectedId_isValid || RepeaterIDs.has($SelectedId)}
      on:click={() => {
        add_repeaterID($SelectedId);
      }}>{$_("app.repeater.ids.add_button") + " " + $SelectedId}</button
    >

    <button
    use:tippy={{ content: $_("app.repeater.ids.tt.remove_button") }}
      type="button"
      disabled={!RepeaterIDs.has(selectedRepId)}
      on:click={() => {
        remove_repeaterID(selectedRepId);
      }}>{$_("app.repeater.ids.remove_button") + " " + (RepeaterIDs.has(selectedRepId) ? selectedRepId : "")}</button
    >
  </div>
</div>

<style lang="scss">
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
