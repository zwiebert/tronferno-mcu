<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { GuiAcc } from "stores/app_state";
  import { G, M0 } from "stores/curr_shutter.js";
  import ShutterGM from "app/shutter_gm.svelte";
  import TransmitterNames from "app/transmitter_names.svelte";
  import { SelectedId, SelectedIdIsValid, TxNames } from "stores/id.js";
  import { Aliases } from "stores/shutters.js";
  import { ReceivedAddresses } from "stores/alias.js";
  import { onMount, onDestroy } from "svelte";
  import tippy from "sveltejs-tippy";

  export const editTxNames = true;

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_ALIASES);
    setTimeout(fetchTxNames, 1000);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: AliasesPairedKeys = Object.keys($Aliases).filter((key) => alias_isKeyPairedToM(key, $G, $M0));
  $: AliasesAllKeys = Object.keys($Aliases).filter((key) => !AliasesPairedKeys.includes(key));
  $: AliasesRxKeys = [...$ReceivedAddresses].filter((key) => id_isValid(key) && !(AliasesPairedKeys.includes(key) || AliasesAllKeys.includes(key)));

  function setSelectedId(id) {
    $SelectedId = id;
  }

  function onChange_Paired() {
    setSelectedId(document.getElementById("paired").value);
  }

  function onChange_All() {
    setSelectedId(document.getElementById("aliases").value);
  }

  function onChange_Rx() {
    setSelectedId(document.getElementById("received").value);
  }

  function sleep(ms) {
    return new Promise((resolve) => setTimeout(resolve, ms));
  }

  async function fetchTxNames() {
    let keys = Object.keys($Aliases);
    const length = keys.length;
    const how_many = 6;

    for (let i = 0; i < length; i += how_many) {
      let tfmcu = { to: "tfmcu", kvs: {} };
      for (let k = i; k < i + how_many && k < length; ++k) {
        tfmcu.kvs["TXN." + keys[k]] = "?";
      }
      httpFetch.http_postCommand(tfmcu, true);
      await sleep(500);
    }
  }

  function id_isValid(id) {
    const re = /^[12]0[0-9A-Fa-f]{4}$/;
    return re.test(id);
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

<div id="aliasdiv">
  <table>
    <tr>
      <td use:tippy={{ content: $_("app.id.tt.chose_regIds") }}>
        <ShutterGM radio={false} groups={false} />
      </td>
      <td
        ><select class="w-full" id="paired" size={AliasesPairedKeys.length} on:change={onChange_Paired} on:click={onChange_Paired}>
          {#each AliasesPairedKeys.sort() as key}
            <option value={key}>{key + " " + ($TxNames[key] || "")}</option>
          {/each}
        </select></td
      >
    </tr>
    <tr>
      <td use:tippy={{ content: $_("app.id.tt.chose_allRegIds") }}>{$_("app.id.chose_allRegIds")}</td>
      <td
        ><select class="w-full" id="aliases" size="1" on:change={onChange_All} on:click={onChange_All}>
          {#each AliasesAllKeys.sort() as key}
            <option value={key}>{key + " " + ($TxNames[key] || "")}</option>
          {/each}
        </select></td
      >
    </tr>

    <tr>
      <td use:tippy={{ content: $_("app.id.tt.chose_rxIds") }}>{$_("app.id.chose_rxIds")}</td>
      <td
        ><select class="w-full" id="received" size="1" on:change={onChange_Rx} on:click={onChange_Rx}>
          {#each AliasesRxKeys.sort() as key}
            <option value={key}>{key + " " + ($TxNames[key] || "")}</option>
          {/each}
        </select></td
      >
    </tr>
    <tr>
      <td use:tippy={{ content: $_("app.id.tt.chose_enterId") }}>{$_("app.id.chose_enterId")}</td>
      <td
        ><label use:tippy={{ content: $_("app.id.tt.text_enterId") }}>
          <input id="entered" class="w-16 {$SelectedIdIsValid ? 'text-green-600' : 'text-red-600'}" type="text" bind:value={$SelectedId} maxlength="6" />
        </label>

        <button
          class="rounded-full"
          type="button"
          use:tippy={{ content: $_("app.id.tt.test_selectedId") }}
          disabled={!$SelectedIdIsValid}
          on:click={() => {
            httpFetch.http_postCommand({ cmd: { a: $SelectedId, c: "sun-test" } });
          }}>{$_("app.id.test_selectedId")}</button
        >
        <button
          class="rounded-full"
          type="button"
          use:tippy={{ content: $_("app.id.tt.send_stop") }}
          disabled={!$SelectedIdIsValid}
          on:click={() => {
            httpFetch.http_postCommand({ cmd: { a: $SelectedId, c: "stop" } });
          }}>{$_("app.id.send_stop")}</button
        >
      </td>
    </tr>

    {#if $GuiAcc.edit_transmitter_names}
      <tr><td use:tippy={{ content: $_("app.id.tt.enterName") }}>{$_("app.navTab.sender.names.input")}</td><td><TransmitterNames edit={editTxNames} /></td></tr>
    {/if}
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
    margin: 0 auto;
    padding: 0rem 0.25rem;
    border-gap: 0;
  }
</style>
