<script>
  "use strict";
  import { _ } from "services/i18n";
  import IdSelector from "app/id_selector.svelte";
  import TransmitterNames from "app/transmitter_names.svelte";
  import { GuiAcc } from "stores/app_state";
  import tippy from "sveltejs-tippy";

  import { Aliases } from "stores/shutters.js";
  import { G, M0 } from "stores/curr_shutter.js";
  import { SelectedId, SelectedIdIsValid } from "stores/id.js";
  import { Gmu, GmuMaxM } from "stores/mcu_config.js";


  $: AliasesPairedKeys = Object.keys($Aliases).filter((key) => alias_isKeyPairedToM(key, $G, $M0));

  $: {
    AliasesPairedKeys;
    aliasTable_updHtml($SelectedId);
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

<div class="main-area text-center">

  <h4 use:tippy={{ content: $_("app.navTab.sender.names.tt.header") }}> {$_("app.navTab.sender.names.header")}</h4>
  <div>
    <IdSelector />
  </div>

  {#if false && $GuiAcc.edit_transmitter_names}
    <div class="main-area flex flex-row">
      {$_("app.navTab.sender.names.input")}
      <TransmitterNames />
    </div>
  {/if}


  <div class="area" id="divPairAll">
    <table id="aliasTable">
      <tr><th colspan="8">{$_("app.id.regTbl_header1") + " " + $SelectedId + " " + $_("app.id.regTbl_header2")}</th></tr>
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
                <td class={$G === 0 || (g === $G && ($M0 === 0 || m === $M0)) ? "bg-selected" : ""}
                  ><input id="cbAlias_{g}{m}" type="checkbox" disabled={true} /></td
                >
              {/if}
            {/each}
          </tr>
        {/if}
      {/each}
    </table>
  </div>
</div>
