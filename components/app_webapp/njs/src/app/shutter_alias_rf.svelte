<script>
  "use strict";
  import { _ } from "../services/i18n";
  import * as httpFetch from "../app/fetch.js";
  import { G, M0, GMH } from "../store/curr_shutter.js";
  import ShutterGM from "../app/shutter_gm.svelte";
  import { SelectedId, SelectedIdIsValid } from "../store/id.js";
  import { Aliases } from "../store/shutters.js";
  import { Pras } from "../store/alias.js";
  import { onMount, onDestroy } from "svelte";
  import tippy from "sveltejs-tippy";





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

  function hClick_Set() {
    httpFetch.http_postShutterCommand("set");
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
  let AliasesAllKeys = $derived(Object.keys($Aliases));
  let AliasesPairedKeys = $derived(AliasesAllKeys.filter((key) => alias_isKeyPairedToM(key, $G, $M0)));
  $effect(() => {
    if ($Pras && $Pras.success) {
      $SelectedId = $Pras.a;
      console.log($Pras);
    }
  });
  
  $effect(() => {
    if (AliasesPairedKeys) {
      aliasTable_updHtml($SelectedId);
    }
  });
</script>

<div id="aliasdiv text-center">
  <div class="area text-center" id="aliasPairUnpair">
    <div class="flex flex-row items-center justify-center">
      <ShutterGM radio={false} /> 
    </div>
    <button id="alias_pair" type="button" onclick={hClick_Pair} use:tippy={{ content: $_("app.id.tt.register_rf") }}>{$_("fernotron.Register")}</button>
    RF-> {$GMH}
    <button id="alias_unpair" type="button" onclick={hClick_UnPair} use:tippy={{ content: $_("app.id.tt.register_rf") }}>{$_("fernotron.Unregister")}</button>
    <br />
    <button class="rounded-full" type="button" onclick={hClick_Set} use:tippy={{ content: $_("app.id.tt.set_function") }}>SET</button>

    {#if $Pras}
      <br />
      {#if $Pras.scanning}
        <span class="bg-blue-400">Press STOP (or SO POS/INST) on Sender now to {$Pras.pairing ? "register" : "unregister"}.</span>
      {:else if $Pras.timeout}
        <span class="bg-red-400">Timeout</span>
      {:else if $Pras.success}
        <label class="bg-green-400">Found: <button type="button" onclick={() => select_id($Pras.a)}>{$Pras.a}</button></label>
      {:else}
        <label class="bg-gray-300">Nothing to do for: <button type="button" onclick={() => select_id($Pras.a)}>{$Pras.a}</button></label>
      {/if}
    {/if}
  </div>
</div>
