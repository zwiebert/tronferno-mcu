<script>
  import { _ } from "../services/i18n";
  import * as httpFetch from "../app/fetch.js";
  import * as misc from "../app/misc.js";
  import { McuFirmwareUpdProgress, McuFirmwareUpdState, McuFirmwareUpdChip, McuFirmwareVersionNumber, McuFirmwareReleaseStatus } from "../store/mcu_firmware";

  ("use strict");
  import { ReloadProgress } from "../store/app_state.js";

  export let fwbtns = [];
  export let chip = "";
  export let updSecs = 30;

  import { onMount, onDestroy } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION | httpFetch.FETCH_GIT_TAGS);
  });

  let netota_intervalID = 0;
  let netota_isInProgress = false;
  let fw_choosen = 0;

  function netota_FetchFeedback() {
    let netmcu = { to: "tfmcu" };
    netmcu.mcu = {};
    netmcu.mcu[$McuFirmwareUpdChip + "ota"] = "?";
    httpFetch.http_postCommand(netmcu);
  }

  function netFirmwareOTA(ota_name) {
    if (netota_isInProgress) return;
    McuFirmwareUpdState.set(0);
    McuFirmwareUpdChip.set(chip);
    let netmcu = { to: "tfmcu" };
    netmcu.mcu = {};
    netmcu.mcu[$McuFirmwareUpdChip + "ota"] = ota_name;
    httpFetch.http_postCommand(netmcu);
    McuFirmwareUpdProgress.set(1);
    netota_intervalID = setInterval(netota_FetchFeedback, 1000);
    netota_isInProgress = true;
  }

  $: {
    if (netota_isInProgress) {
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

<div class="area">
  <h5 class="text-center">Installed Firmware</h5>
  <ul>
    <li>{$_("firmware.version")}: {$McuFirmwareVersionNumber}</li>
    <li>Release Status: {$McuFirmwareReleaseStatus}</li>
  </ul>
</div>

<div class="area" id="id-fwDiv">
  <h5 class="text-center">Download and Flash New Firmware (OTA)</h5>
    <dl>
      {#each fwbtns as bt, i}
        <dt>
          <input
            checked={fw_choosen === i}
            on:change={() => {
              fw_choosen = i;
            }}
            type="radio"
            name="amount"
            value={i}
            disabled={$McuFirmwareUpdState === 1}
          />
          {#if bt.input === "none"}
            {bt.name} {bt.version}
          {:else if bt.input === "select"}
            {bt.name} 
            <select bind:value={bt.value}>
              {#each bt.values as name}
                <option value={name}>{name}</option>
              {/each}
            </select>
          {:else if bt.input === "input"}
            URL:
            <input type="text" id={bt.ota_name} bind:value={bt.value} />
          {/if}
        </dt>
      {/each}
    </dl>
    <button type="button" disabled={fw_choosen === -1 || $McuFirmwareUpdState === 1} on:click={() => netFirmwareOTA(fwbtns[fw_choosen].get_ota_name())}> Start Update </button>

  {#if $McuFirmwareUpdChip === chip}
    {#if $McuFirmwareUpdState === 2}
      <br />
      <strong> Update failed <br /> <br /> </strong>
    {:else if $McuFirmwareUpdState === 3}
      <br />
      <strong>
        {$_("app.msg_firmwareUpdSuccess")}
        <button id="mrtb" type="button" on:click={() => misc.req_mcuRestart()}>
          {$_("app.restartMcu")}
        </button>
        <br />
        <br />
      </strong>
    {:else if $McuFirmwareUpdState === 1}
      <strong>{$_("app.msg_firmwareIsUpdating")}</strong>
      <br />
      <br />
      <progress value={$McuFirmwareUpdProgress} max={updSecs * 2} />
    {/if}
    {#if $ReloadProgress > 0}
      <strong>{$_("app.msg_waitForMcuRestart")}</strong>
      <br />
      <progress value={$ReloadProgress} max="100" />
    {/if}
  {/if}
</div>

<style lang="scss">
  @import "../styles/app.scss";
  table,
  th,
  td {
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
