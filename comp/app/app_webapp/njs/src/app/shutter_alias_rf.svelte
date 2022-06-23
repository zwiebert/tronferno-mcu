<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GMH } from "stores/curr_shutter.js";
  import ShutterGM from "app/shutter_gm.svelte";
  import { SelectedId } from "stores/id.js";
  import { Aliases } from "stores/shutters.js";
  import { Pras } from "stores/alias.js";
  import { onMount, onDestroy } from "svelte";
  import tippy from "sveltejs-tippy";


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



</script>

<div id="aliasdiv text-center">


  <div class="area text-center" id="aliasPairUnpair">
    <ShutterGM radio={false} />

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
