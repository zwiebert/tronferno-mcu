<script>
  "use strict";
  import { _ } from "../services/i18n";
  import * as httpFetch from "../app/fetch.js";
  import { SelectedId, TxNames } from "../store/id.js";
  import { McuConfig } from "../store/mcu_config.js";
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
  let mcuConfig = $derived($McuConfig);
  let RepeaterIDs = $derived(new Set("rf-repeater" in mcuConfig ? $McuConfig["rf-repeater"].match(/.{6}/g) : []));
  let selectedId_isValid = $derived(id_isValid($SelectedId));
  let selectedRepId = $state("");
  
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
      onclick={() => {
        add_repeaterID($SelectedId);
      }}>{$_("app.repeater.ids.add_button") + " " + $SelectedId}</button
    >

    <button
    use:tippy={{ content: $_("app.repeater.ids.tt.remove_button") }}
      type="button"
      disabled={!RepeaterIDs.has(selectedRepId)}
      onclick={() => {
        remove_repeaterID(selectedRepId);
      }}>{$_("app.repeater.ids.remove_button") + " " + (RepeaterIDs.has(selectedRepId) ? selectedRepId : "")}</button
    >
  </div>
</div>

