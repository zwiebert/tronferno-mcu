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

  $: selectedId_isValid = id_isValid(selectedId);

  $: {
    aliasTable_updHtml(selectedId);
  }

  function id_isValid(id) {
    const re = /^[12]0[0-9A-Fa-f]{4}$/;
    return re.test(id);
  }

  function select_id(id) {
    $SelectedId = id;
  }



</script>



<div id="aliasdiv text-center">

  <br />

  <div class="area text-center">
 

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

    <div class="area">
      <h5>Alias {selectedId} -> {$GMH}</h5>
      <button
        type="button"
        disabled={!selectedId_isValid}
        on:click={() => {
          httpFetch.http_postCommand({ pair: { a: selectedId, g: $G, m: $M0, c: "pair" } });
        }}>Add</button
      >

      <button
        type="button"
        disabled={!selectedId_isValid}
        on:click={() => {
          httpFetch.http_postCommand({ pair: { a: selectedId, g: $G, m: $M0, c: "unpair" } });
        }}>Remove</button
      >
    </div>
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
