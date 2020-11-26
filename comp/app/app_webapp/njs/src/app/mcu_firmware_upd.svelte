<script>
  import { _ } from "services/i18n";
  import * as appDebug from "app/app_debug.js";
  import * as httpFetch from "app/fetch.js";
  import * as misc from "app/misc.js";
  import { McuFirmwareUpdProgress, McuFirmwareUpdState, McuFirmwareUpdChip } from "stores/mcu_firmware";

  ("use strict");
  import { ReloadProgress } from "stores/app_state.js";

  export let fwbtns = [];
  export let chip = "";
  export let updSecs = 30;

  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GIT_TAGS);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  let netota_intervalID = 0;
  let netota_isInProgress = false;

  function netota_FetchFeedback() {
    let netmcu = { to: "tfmcu" };
    netmcu.mcu = {};
    netmcu.mcu[$McuFirmwareUpdChip + "ota"] = "?";
    let url = "/cmd.json";
    appDebug.dbLog("url: " + url);
    httpFetch.http_postRequest(url, netmcu);
  }

  function netFirmwareOTA(ota_name) {
    if (netota_isInProgress) return;
    McuFirmwareUpdState.set(0);
    McuFirmwareUpdChip.set(chip);
    let netmcu = { to: "tfmcu" };
    netmcu.mcu = {};
    netmcu.mcu[$McuFirmwareUpdChip + "ota"] = ota_name;
    let url = "/cmd.json";
    appDebug.dbLog("url: " + url);
    httpFetch.http_postRequest(url, netmcu);
    McuFirmwareUpdProgress.set(1);
    netota_intervalID = setInterval(netota_FetchFeedback, 1000);
    netota_isInProgress = true;
  }

  $: {
    if (netota_isInProgress) {
      console.log("progress trace", $McuFirmwareUpdState);
      if ($McuFirmwareUpdState === 0) {
        //
      } else if ($McuFirmwareUpdState === 1) {
        //
      } else {
        clearInterval(netota_intervalID);
        netota_isInProgress = false;
      }
    }
  }
</script>

<style type="text/scss">
  @import "../styles/app.scss";
  table,
  th,
  td {
    border-color: $color_border_main_area;
    border-style: solid;
    margin: 0rem;
    padding: 0rem;
    border-gap: 0;
  }
</style>

<div id="id-fwDiv">
  {#if $McuFirmwareUpdState !== 1}
    <table class="border-solid border-collapse">
      <th>Firmware</th>
      <th>Action</th>
      {#each fwbtns as bt}
        <tr>
          {#if bt.input === 'none'}
            <td class="text-center">{bt.name} {bt.version}</td>
          {:else if bt.input === 'select'}
            <td>
              {bt.name}
              <select bind:value={bt.value}>
                {#each bt.values as name}
                  <option value={name}>{name}</option>
                {/each}
              </select>
            </td>
          {:else if bt.input === 'input'}
            <td class="text-center">URL: <input type="text" id={bt.ota_name} bind:value={bt.value} /></td>
          {/if}
          <td><button type="button" on:click={() => netFirmwareOTA(bt.get_ota_name())}> Update </button></td>
        </tr>
      {/each}
    </table>
  {/if}

  {#if $McuFirmwareUpdChip === chip}
    {#if $McuFirmwareUpdState === 2}
      <br />
      <strong> Update failed <br /> <br /> </strong>
    {:else if $McuFirmwareUpdState === 3}
      <br />
      <strong> Update succeeded <button id="mrtb" type="button" on:click={() => misc.req_mcuRestart()}> {$_('app.restartMcu')} </button> <br /> <br /> </strong>
    {:else if $McuFirmwareUpdState === 1}
      <strong>{$_('app.msg_firmwareIsUpdating')}</strong>
      <br />
      <br />
      <progress value={$McuFirmwareUpdProgress} max={updSecs} />
    {/if}
    {#if $ReloadProgress > 0}<strong>{$_('app.msg_waitForMcuRestart')}</strong> <br /> <progress value={$ReloadProgress} max="100" />{/if}
  {/if}
</div>
