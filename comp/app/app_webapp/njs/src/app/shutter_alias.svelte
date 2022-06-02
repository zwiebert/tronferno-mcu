<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GMH } from "stores/curr_shutter.js";
  import { SelectedId } from "stores/id.js";
  import { Gmu, GmuMaxM } from "stores/mcu_config.js";
  import { Aliases } from "stores/shutters.js";
  import { Pras } from "stores/alias.js";
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
  $: selectedId = $SelectedId;
  $: selectedId_isValid = id_isValid(selectedId);

  $: isPaired = false;
  $: {
    AliasesPairedKeys;
    isPaired = alias_isKeyPairedToM($SelectedId, $G, $M0);
  }

  $: {
    $Pras;
    if ($Pras && $Pras.success) {
      selectedId = $SelectedId = $Pras.a;
      console.log($Pras);
    }
  }

  $: {
    AliasesPairedKeys;
    aliasTable_updHtml(selectedId);
  }

  function id_isValid(id) {
    const re = /^[12]0[0-9A-Fa-f]{4}$/;
    return re.test(id);
  }

  function select_id(id) {
    $SelectedId = id;
  }

  function hClick_Pair() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES_START_PAIRING);
  }

  function hClick_UnPair() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES_START_UNPAIRING);
  }

  function onAliasesApply() {
    if (selectedId_isValid) {
      aliasTable_fromHtml_toMcu(selectedId);
    }
  }
  function onAliasesReload() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES);
  }

  function alias_isKeyPairedToM(key, g, m) {
    const val = $Aliases[key] || {};

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
    const val = $Aliases[key] || {};
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
  function aliasTable_fromHtml() {
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
    const val = aliasTable_fromHtml();

    let tfmcu = { to: "tfmcu", pair: { a: key, mm: val, c: "store" } };

    httpFetch.http_postCommand(tfmcu);
    httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES);
  }
</script>

<div id="aliasdiv text-center">
  <br />

  <br />

  <div class="area" id="divPairAll">
    <table id="aliasTable">
      <tr><th colspan="8">{$_("app.id.regTbl_header1") + " " + selectedId + " " +$_("app.id.regTbl_header2") }</th></tr>
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

  <div class="area">
    <button
      type="button"
      disabled={!selectedId_isValid || isPaired}
      use:tippy={{ content: $_("app.id.tt.register_id") }}
      on:click={() => {
        httpFetch.http_postCommand({ pair: { a: selectedId, g: $G, m: $M0, c: "pair" } });
        httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES);
      }}>{$_("fernotron.Register")}</button
    >
    {selectedId} -> {$GMH}
    <button
      type="button"
      disabled={!selectedId_isValid || !isPaired}
      use:tippy={{ content: $_("app.id.tt.register_id") }}
      on:click={() => {
        httpFetch.http_postCommand({ pair: { a: selectedId, g: $G, m: $M0, c: "unpair" } });
        httpFetch.http_fetchByMask(httpFetch.FETCH_ALIASES);
      }}>{$_("fernotron.Unregister")}</button
    >
  </div>
</div>
<div class="area text-center" id="aliasPairUnpair">
  <button id="alias_pair" type="button" on:click={hClick_Pair} use:tippy={{ content: $_("app.id.tt.register_rf") }}>{$_("fernotron.Register")}</button>
  RF-> {$GMH}
  <button id="alias_unpair" type="button" on:click={hClick_UnPair} use:tippy={{ content: $_("app.id.tt.register_rf") }}>{$_("fernotron.Unregister")}</button>
  {#if $Pras}
    <br />
    {#if $Pras.scanning}
      <span class="bg-blue-400">Press STOP (or SO POS/INST) on Sender now to {$Pras.pairing ? "register" : "unregister"}.</span>
    {:else if $Pras.timeout}
      <span class="bg-red-400">Timeout</span>
    {:else if $Pras.success}
      <label class="bg-green-400">Found: <button type="button" on:click={() => select_id($Pras.a)}>{$Pras.a}</button></label>
    {:else}
      <label class="bg-gray-300">Nothing to do for: <button type="button" on:click={() => select_id($Pras.a)}>{$Pras.a}</button></label>
    {/if}
  {/if}
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
    margin: 0 auto;
    padding: 0rem 0.25rem;
    border-gap: 0;
  }
</style>
