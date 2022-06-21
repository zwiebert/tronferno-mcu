<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GMH } from "stores/curr_shutter.js";
  import ShutterGM from "app/shutter_gm.svelte";
  import { SelectedId } from "stores/id.js";
  import { Aliases } from "stores/shutters.js";
  import { ReceivedAddresses } from "stores/alias.js";
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

  $: {
    select_id($SelectedId);
  }

  function id_isValid(id) {
    const re = /^[12]0[0-9A-Fa-f]{4}$/;
    return re.test(id);
  }

  let select_id_in_progress = false;
  function select_id(id) {
    if (select_id_in_progress) return;
    select_id_in_progress = true;

    for (let eid of ["paired", "aliases", "received", "entered"]) {
      let el = document.getElementById(eid);
      if (el) el.value = id;
    }
    selectedId = id;
    $SelectedId = id;
    select_id_in_progress = false;
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
  <table class="top_table ml-auto mr-auto">
    <tr><th colspan="4">{$_("app.id.chose_header")}</th></tr>
    <tr>
      <td use:tippy={{ content: $_("app.id.tt.chose_allRegIds") }}>{$_("app.id.chose_allRegIds")}</td>
      <td use:tippy={{ content: $_("app.id.tt.chose_regIds") }}><ShutterGM radio={false} /></td>
      <td use:tippy={{ content: $_("app.id.tt.chose_rxIds") }}>{$_("app.id.chose_rxIds")}</td>
      <td use:tippy={{ content: $_("app.id.tt.chose_enterId") }}>{$_("app.id.chose_enterId")}</td>
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
        <label use:tippy={{ content: $_("app.id.tt.text_enterId") }}>
          <input
            id="entered"
            class="w-16 {selectedId_isValid ? 'text-green-600' : 'text-red-600'}"
            type="text"
            on:input={() => select_id(document.getElementById("entered").value)}
            maxlength="6"
          />
        </label>
        <br />
        <button
          type="button"
          use:tippy={{ content: $_("app.id.tt.test_selectedId") }}
          disabled={!selectedId_isValid}
          on:click={() => {
            httpFetch.http_postCommand({ cmd: { a: selectedId, c: "sun-test" } });
          }}>{$_("app.id.test_selectedId")}</button
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
    margin: 0 auto;
    padding: 0rem 0.25rem;
    border-gap: 0;
  }
</style>
