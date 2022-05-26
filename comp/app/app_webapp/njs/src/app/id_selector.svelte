<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GM, GMH } from "stores/curr_shutter.js";
  import { SelectedId } from "stores/id.js";
  import { Gmu, GmuMaxM } from "stores/mcu_config.js";
  import { Aliases } from "stores/shutters.js";
  import { ShowHelp } from "stores/app_state.js";
  import { Pras, ReceivedAddresses } from "stores/alias.js";
  import { onMount, onDestroy } from "svelte";
  import tippy from "sveltejs-tippy";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_ALIASES);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: AliasesAllKeys = Object.keys($Aliases);
  $: AliasesPairedKeys = AliasesAllKeys.filter((key) => alias_isKeyPairedToM(key, $G, $M0));
  $: selectedId = 0;
  $: selectedId_isValid = id_isValid(selectedId);

  function id_isValid(id) {
    const re = /^[12]0[0-9A-Fa-f]{4}$/;
    return re.test(id);
  }

  let select_id_in_progress = false;
  function select_id(id) {
    console.log("id,entered", id, entered);
    if (select_id_in_progress) return;
    select_id_in_progress = true;

    for (let eid of ["paired", "aliases", "received", "entered"]) {
      document.getElementById(eid).value = id;
    }
    selectedId = id;
    $SelectedId = id;
    select_id_in_progress = false;
  }

  function hClick_Pair() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES_START_PAIRING);
  }

  function hClick_UnPair() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES_START_UNPAIRING);
  }

  function aliasControllers_fromHtml() {
    // XXX
    const pad = $Aliases;
    const pas = document.getElementById("aliases");
    const pas_sel = pas.selectedIndex;

    for (let opt of pas.options) {
      const key = opt.text;
      aliasTable_fromHtml(key);
    }
  }

  function onAliasesApply() {
    if (selectedId_isValid) {
     // aliasTable_fromHtml_toMcu(selectedId);
    }
  }
  function onAliasesReload() {
   // aliasTable_updHtml(selectedId);
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES);
  }

  function alias_isKeyPairedToM(key, g, m) {
    const val = $Aliases[key];

    let chunks = [];

    for (let i = 0, charsLength = val.length; i < charsLength; i += 2) {
      chunks.unshift(val.substring(i, i + 2));
    }

    const g_max = chunks.length - 1;

    if (g > g_max) return false;

    const b = parseInt(chunks[g], 16);

    return (b & (1 << m)) !== 0;
  }

  
</script>

<div id="aliasdiv" class="text-center">
    <h5>Select or Enter an ID</h5>
    <table class="top_table">
      <tr>
        <td use:tippy={{ content: "All aliased IDs" }}>All</td>
        <td use:tippy={{ content: "IDs aliased to this number" }}>{$GMH}</td>
        <td use:tippy={{ content: "Any received IDs" }}>RX</td>
        <td use:tippy={{ content: "Enter an ID by hand" }}>Enter</td>
      </tr>
      <tr>
        <td>
          <select id="aliases" size="5" on:change={() => select_id(document.getElementById("aliases").value)}>
            {#each AliasesAllKeys.sort() as key}
              <option>{key}</option>
            {/each}
          </select>
        </td>
        <td>
          <select id="paired" size="5" on:change={() => select_id(document.getElementById("paired").value)}>
            {#each AliasesPairedKeys.sort() as key}
              <option>{key}</option>
            {/each}
          </select>
        </td>
        <td>
          <select id="received" size="5" on:change={() => select_id(document.getElementById("received").value)}>
            {#each [...$ReceivedAddresses].sort() as key}
              <option>{key}</option>
            {/each}
          </select>
        </td>
        <td>
          <label use:tippy={{ content: "Enter an ID or select one from a list" }}>
            <input
              id="entered"
              class="w-16 {selectedId_isValid ? 'text-green-600' : 'text-red-600'}"
              type="text"
              on:input={() => select_id(document.getElementById("entered").value)}
              maxlength="6"
            />
          </label>
          <br>
          <button
            type="button"
            use:tippy={{ content: "Identify motor(s) paired with this ID by moving them" }}
            disabled={!selectedId_isValid}
            on:click={() => {
              httpFetch.http_postCommand({ cmd: { a: selectedId, c: "sun-test" } });
            }}>Test</button
          >
        </td>
      </tr>
    </table>
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
