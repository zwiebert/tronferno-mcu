<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GM, GMH } from "stores/curr_shutter.js";
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
  $: AliasSelectedKey = 0;
  $: selectedId = 0;
  $: enteredId = 0;
  $: selectedId_isValid = id_isValid(selectedId);

  $: {
    aliasTable_updHtml(selectedId);
  }

  let selectedReceivedAddress = 0;
  let selectedThisPairings = 0;

  function id_isValid(id) {
    const ids = id;

    return ids.length === 6 && (ids.startsWith("10") || ids.startsWith("20"));
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
      aliasTable_fromHtml_toMcu(selectedId);
    }
  }
  function onAliasesReload() {
    aliasTable_updHtml(selectedId);
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

  function aliasTable_updHtml(key) {
    const val = $Aliases[key];
    if (!val) {
      for (let g = 1; g <= 7; ++g) {
        for (let m = 1; m <= 7; ++m) {
          let cb = document.getElementById("cbAlias_" + g.toString() + m.toString());
          if (cb) {
            cb.checked = false;
          }
        }
      }
      return;
    }
    let chunks = [];

    for (let i = 0, charsLength = val.length; i < charsLength; i += 2) {
      chunks.unshift(val.substring(i, i + 2));
    }

    const g_max = chunks.length - 1;

    for (let g = 1; g <= 7; ++g) {
      const gn = g > g_max ? 0 : parseInt(chunks[g], 16);

      for (let m = 1; m <= 7; ++m) {
        const isAliased = (gn & (1 << m)) !== 0;
        let cb = document.getElementById("cbAlias_" + g.toString() + m.toString());
        if (cb) {
          cb.checked = isAliased;
        }
      }
    }
  }
  function aliasTable_fromHtml(key) {
    let val = "00";
    let null_run = 0;

    for (let g = 1; g <= 7; ++g) {
      let gn = 0;
      for (let m = 1; m <= 7; ++m) {
        let cb = document.getElementById("cbAlias_" + g.toString() + m.toString());
        const isAliased = cb && cb.checked;
        if (isAliased) {
          gn |= 1 << m;
        }
      }
      if (gn === 0) {
        ++null_run;
      } else {
        for (; null_run > 0; --null_run) {
          val = "00" + val;
        }
        const hex = gn.toString(16);
        val = hex + val;
        if (hex.length < 2) val = "0" + val;
      }
    }
    return val;
  }

  function aliasTable_fromHtml_toMcu(key) {
    const val = aliasTable_fromHtml(key);

    let tfmcu = { to: "tfmcu", pair: { a: key, mm: val, c: "store" } };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES);
  }
</script>

<div id="aliasdiv">
  <p class={$ShowHelp}>{$_("help.hint_shutterAlias")} <br /></p>
  <div class="area text-center" id="aliasPairUnpair">
    <button id="alias_pair" type="button" on:click={hClick_Pair}> Scan for ID to alias it to {$GMH} </button>
    <button id="alias_unpair" type="button" on:click={hClick_UnPair}> Scan for ID to unalias it from {$GMH} </button>
    {#if $Pras}
      <br />
      {#if $Pras.scanning}
        <span class="bg-blue-400">Scanning to {$Pras.pairing ? "alias" : "unalias"}. Press STOP (or SO POS/INST) on Sender now.</span>
      {:else if $Pras.timeout}
        <span class="bg-red-400">Timeout</span>
      {:else if $Pras.success}
        <label class="bg-green-400"
          >Found: <button type="button" on:click={() => (selectedId = selectedReceivedAddress = AliasSelectedKey = selectedThisPairings = $Pras.a)}
            >{$Pras.a}</button
          ></label
        >
      {:else}
        <label class="bg-gray-300"
          >Nothing to do for: <button type="button" on:click={() => (selectedId = selectedReceivedAddress = AliasSelectedKey = selectedThisPairings = $Pras.a)}
            >{$Pras.a}</button
          ></label
        >
      {/if}
    {/if}
  </div>
  <br />

  <div class="area text-center">
    <h5>Select or Enter an ID</h5>
    <table class="top_table">
      <tr>
        <td use:tippy={{ content: "All aliased IDs" }}>All</td>
        <td use:tippy={{ content: "IDs aliased to this number" }}>{$GM}</td>
        <td use:tippy={{ content: "Any received IDs" }}>RX</td>
        <td use:tippy={{ content: "Enter an ID by hand" }}>Enter</td>
      </tr>
      <tr>
        <td>
          <select
            id="aliases"
            size="5"
            bind:value={AliasSelectedKey}
            on:click={() => (selectedId = selectedReceivedAddress = selectedThisPairings = AliasSelectedKey)}
          >
            {#each AliasesAllKeys.sort() as key}
              <option>{key}</option>
            {/each}
          </select>
        </td>
        <td>
          <select
            id="paired"
            size="5"
            bind:value={selectedThisPairings}
            on:click={() => (selectedId = selectedReceivedAddress = AliasSelectedKey = selectedThisPairings)}
          >
            {#each AliasesPairedKeys.sort() as key}
              <option>{key}</option>
            {/each}
          </select>
        </td>
        <td>
          <select
            size="5"
            bind:value={selectedReceivedAddress}
            on:click={() => (selectedId = selectedThisPairings = AliasSelectedKey = selectedReceivedAddress)}
          >
            {#each [...$ReceivedAddresses].sort() as key}
              <option>{key}</option>
            {/each}
          </select>
        </td>
        <td>
          <label use:tippy={{ content: "Enter ID or select one in list above" }}
            >ID:
            <input class="w-16 {selectedId_isValid ? 'text-green-600' : 'text-red-600'}" type="text" bind:value={selectedId} maxlength="6"/>
          </label>
        </td>
      </tr>
    </table>

    <br />

    <div class="area" id="divPairAll">
      <h5>ID {selectedId} is aliased to:</h5>
      <table id="aliasTable">
        <tr>
          <th />
          {#each [1, 2, 3, 4, 5, 6, 7] as m}
            {#if m <= $GmuMaxM}
              <th>{m}</th>
            {/if}
          {/each}
        </tr>
        {#each [1, 2, 3, 4, 5, 6, 7] as g}
          {#if $Gmu[g] > 0}
            <tr>
              <th>g{g}</th>
              {#each [1, 2, 3, 4, 5, 6, 7] as m}
                {#if m <= $Gmu[g]}
                  <td><input id="cbAlias_{g}{m}" type="checkbox" disabled={!selectedId_isValid} /></td>
                {/if}
              {/each}
            </tr>
          {/if}
        {/each}
      </table>
      <span id="aliasSaveReload">
        <button id="alias_reload" type="button" on:click={onAliasesReload}> {$_("app.reload")} </button>
        <button id="alias_save" type="button" on:click={onAliasesApply}> {$_("app.save")} </button>
      </span>
    </div>

    <div class="area text-center">
      <h5>Send Commands as {selectedId}</h5>
      <button
        type="button"
        use:tippy={{ content: "Identify motor(s) paired with this ID by moving them" }}
        disabled={!(selectedId.toString().startsWith("10") || selectedId.toString().startsWith("20"))}
        on:click={() => {
          httpFetch.http_postRequest("/cmd.json", { cmd: { a: selectedId, c: "sun-test" } });
        }}>Test</button
      >

      <button
        type="button"
        disabled={!selectedId.toString().startsWith("20")}
        on:click={() => {
          httpFetch.http_postRequest("/cmd.json", { cmd: { a: selectedId, c: "sun-inst" } });
        }}>Sun-Pos/Inst</button
      >

      <button
        type="button"
        disabled={!selectedId.toString().startsWith("10")}
        on:click={() => {
          httpFetch.http_postRequest("/cmd.json", { cmd: { a: selectedId, c: "stop" } });
        }}>STOP</button
      >
    </div>

    <div class="area">
      <h5>Alias {selectedId} -> {$GMH}</h5>
      <button
        type="button"
        disabled={!(selectedId.toString().startsWith("10") || selectedId.toString().startsWith("20"))}
        on:click={() => {
          httpFetch.http_postRequest("/cmd.json", { pair: { a: selectedId, g: $G, m: $M0, c: "pair" } });
        }}>Add</button
      >

      <button
        type="button"
        disabled={!(selectedId.toString().startsWith("10") || selectedId.toString().startsWith("20"))}
        on:click={() => {
          httpFetch.http_postRequest("/cmd.json", { pair: { a: selectedId, g: $G, m: $M0, c: "unpair" } });
        }}>Remove</button
      >
    </div>
  </div>
</div>

<style type="text/scss">
  @import "../styles/app.scss";
  #aliasHeader {
    grid-area: ah;
  }
  #aliases {
    grid-area: as;
  }
  #divPairAll {
    grid-area: at;
  }
  #aliasSaveReload {
    grid-area: ar;
  }
  #aliasPairUnpair {
    grid-area: ap;
  }

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
