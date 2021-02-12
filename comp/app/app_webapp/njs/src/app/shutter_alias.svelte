<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GM } from "stores/curr_shutter.js";
  import { Gmu, GmuMaxM } from "stores/mcu_config.js";
  import { Aliases } from "stores/shutters.js";
  import { ShowHelp } from "stores/app_state.js";

  import { onMount, onDestroy } from "svelte";

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
  $: {
    aliasTable_updHtml(AliasSelectedKey);
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
    const pas = document.getElementById("aliases");
    if (pas.selectedIndex >= 0) {
      const key = pas.options[pas.selectedIndex].text;
      aliasTable_fromHtml_toMcu(key);
    }
  }
  function onAliasesReload() {
    aliasTable_updHtml(AliasSelectedKey);
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
    if (!val)
     return;
    let chunks = [];

    for (let i = 0, charsLength = val.length; i < charsLength; i += 2) {
      chunks.unshift(val.substring(i, i + 2));
    }

    const g_max = chunks.length - 1;

    for (let g = 1; g <= 7; ++g) {
      const gn = g > g_max ? 0 : parseInt(chunks[g], 16);

      for (let m = 1; m <= 7; ++m) {
        const isAliased = (gn & (1 << m)) !== 0;
        let cb = document.getElementById(
          "cbAlias_" + g.toString() + m.toString()
        );
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

<style type="text/scss">
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
</style>

<div id="aliasdiv">
  <p class={$ShowHelp}>{$_('help.hint_shutterAlias')} <br /></p>
  <span id="aliasPairUnpair">
    <button id="alias_pair" type="button" on:click={hClick_Pair}> Start Pairing </button>
    <button id="alias_unpair" type="button" on:click={hClick_UnPair}> Start Un-Pairing </button>
  </span>
  <br />
  <table class="top_table">
    <tr>
      <th>All</th>
      <th>This G/E</th>
    </tr>
    <tr>
      <td>
        <select id="aliases" size="5" bind:value={AliasSelectedKey}>
          {#each AliasesAllKeys as key}
            <option>{key}</option>
          {/each}
        </select>
      </td>
      <td>
        <select id="paired" size="5" >
          {#each AliasesPairedKeys as key}
            <option>{key}</option>
          {/each}
        </select>
      </td>
    </tr>
  </table>

  <div id="divPairAll">
    <table id="aliasTable">
      <tr>
        <th />
        {#each [1, 2, 3, 4, 5, 6, 7] as m}
          {#if m <= $GmuMaxM}
            <th>m{m}</th>
          {/if}
        {/each}
      </tr>
      {#each [1, 2, 3, 4, 5, 6, 7] as g}
        {#if $Gmu[g] > 0}
          <tr>
            <th>g{g}</th>
            {#each [1, 2, 3, 4, 5, 6, 7] as m}
              {#if m <= $Gmu[g]}
                <td><input id="cbAlias_{g}{m}" type="checkbox" /></td>
              {/if}
            {/each}
          </tr>
        {/if}
      {/each}
    </table>
  </div>

  <span id="aliasSaveReload">
    <button id="alias_reload" type="button" on:click={onAliasesReload}> {$_('app.reload')} </button>
    <button id="alias_save" type="button" on:click={onAliasesApply}> {$_('app.save')} </button>
  </span>
</div>
