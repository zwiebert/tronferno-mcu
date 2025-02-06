<!-- @migration-task Error while migrating Svelte code: `<tr>` cannot be a child of `<table>`. `<table>` only allows these children: `<caption>`, `<colgroup>`, `<tbody>`, `<thead>`, `<tfoot>`, `<style>`, `<script>`, `<template>`. The browser will 'repair' the HTML (by moving, removing, or inserting elements) which breaks Svelte's assumptions about the structure of your components. -->
<script>
  "use strict";
  import { _ } from "../services/i18n";
  import * as httpFetch from "../app/fetch.js";
  import { G, M0 } from "../store/curr_shutter.js";
  import { SelectedId, SelectedIdIsValid, TxName } from "../store/id.js";
  import { Gmu, GmuMaxM } from "../store/mcu_config.js";
  import { Aliases } from "../store/shutters.js";
  import { Pras } from "../store/alias.js";
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

  function rxSymbol(id) {
    return id.startsWith("20") ? "\u263C " : "\u2195 ";
  }
</script>

<div id="aliasdiv text-center">
  <div class="area" id="divPairAll" style={$SelectedIdIsValid ? "" : "visibility:hidden"}>
    <h5 use:tippy={{ content: $_("app.id.tt.registeredToTheseRx") }}>{rxSymbol(selectedId) + selectedId + " " + $TxName}</h5>
    <table id="aliasTable">
      <thead>
        <tr>
          <th />
          {#each [1, 2, 3, 4, 5, 6, 7] as m}
            {#if m <= $GmuMaxM}
              <th>{m}</th>
            {/if}
          {/each}
        </tr>
      </thead>
      {#each [1, 2, 3, 4, 5, 6, 7] as g}
        {#if $Gmu[g] > 0}
          <tbody>
            <tr>
              <th>g{g}</th>
              {#each [1, 2, 3, 4, 5, 6, 7] as m}
                {#if m <= $Gmu[g]}
                  <td class={$G === 0 || (g === $G && ($M0 === 0 || m === $M0)) ? "bg-selected" : ""}>
                    <input id="cbAlias_{g}{m}" type="checkbox" disabled={!selectedId_isValid} />
                  </td>
                {/if}
              {/each}
            </tr>
          </tbody>
        {/if}
      {/each}
    </table>
    <span id="aliasSaveReload">
      <button id="alias_reload" type="button" on:click={onAliasesReload}> {$_("app.reload")} </button>
      <button id="alias_save" type="button" on:click={onAliasesApply}> {$_("app.save")} </button>
    </span>
  </div>
</div>

<style lang="scss">
  @use "../styles/app.scss" as *;

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
