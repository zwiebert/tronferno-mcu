<script>
  import { _ } from "../services/i18n";
  import * as httpFetch from "../app/fetch.js";
  import * as misc from "../app/misc.js";
  import { McuFirmwareUpdProgress, McuFirmwareUpdState, McuFirmwareUpdChip, McuFirmwareVersionNumber, McuFirmwareReleaseStatus } from "../store/mcu_firmware";

  ("use strict");
  import { ReloadProgress } from "../store/app_state.js";


  import { onMount, onDestroy } from "svelte";
  /**
   * @typedef {Object} Props
   * @property {any} [fwbtns]
   * @property {string} [chip]
   */

  /** @type {Props} */
  let { fwbtns = [], chip = "" } = $props();

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION | httpFetch.FETCH_GIT_TAGS);
  });

  let netota_intervalID = $state(0);
  let netota_isInProgress = $state(false);

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

  function compare_version(a, b) {
    try {
      if (!a || !b) return null;

      if (a === b) return 0;
      let as = a.split(".");
      let bs = b.split(".");
      for (let i = 0; i < as.length && i < bs.length; ++i) {
        let an = parseInt(as[i]);
        let bn = parseInt(bs[i]);
        if (an < bn) {
          return -1;
        }
        if (an > bn) {
          return 1;
        }
      }
      if (as.length < bs.length) {
        return -1;
      }
    } catch (e) {
      return null;
    }
    return 1;
  }
 
  let fw_choosen_usr = $state(-1);
  let fw_choosen = $derived(fw_choosen_usr >= 0 ? fw_choosen_usr : ($McuFirmwareReleaseStatus === "beta" ? 1 : 0) );

  function get_ota_name() {
    console.log("fwbtns",fwbtns);
    //return netFirmwareOTA(fwbtns[fw_choosen].get_ota_name());
  
  } 
  let fw_comp = $derived(compare_version($McuFirmwareVersionNumber, fwbtns[fw_choosen].version));
  let fw_updown = $derived(fw_comp < 0 ? "Upgrade" : fw_comp > 0 ? "Downgrade" : fw_comp === 0 ? "Re-Install" : "Install");
  $effect(() => {
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
  });
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
          onchange={() => {
            fw_choosen_usr = i;
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
          <select bind:value={bt.version} disabled={fw_choosen !== i}>
            {#each bt.values as name}
              <option value={name}>{name}</option>
            {/each}
          </select>
        {:else if bt.input === "input"}
          URL:
          <input type="text" id={bt.ota_name} bind:value={bt.url} disabled={fw_choosen !== i} />
        {/if}
      </dt>
    {/each}
  </dl>
  {#if fw_comp !== null}
  <span class="block">
    {fw_updown} {$McuFirmwareVersionNumber} => {fwbtns[fw_choosen].version}
  </span>
  {/if}

  <button type="button" disabled={fw_choosen < 0 || $McuFirmwareUpdState === 1} onclick={() => netFirmwareOTA(fwbtns[fw_choosen].get_ota_name())}>
    Start Download and Installation
  </button>

  {#if $McuFirmwareUpdChip === chip}
    {#if $McuFirmwareUpdState === 2}
      <br />
      <strong> Update failed <br /> <br /> </strong>
    {:else if $McuFirmwareUpdState === 3}
      <br />
      <strong>
        {$_("app.msg_firmwareUpdSuccess")}
        <button id="mrtb" type="button" onclick={() => misc.req_mcuRestart()}>
          {$_("app.restartMcu")}
        </button>
        <br />
        <br />
      </strong>
    {:else if $McuFirmwareUpdState === 1}
      <div class="loader"></div>
      <br />
      <strong>{$_("app.msg_firmwareIsUpdating")}</strong>
    {/if}
    {#if $ReloadProgress > 0}
      <strong>{$_("app.msg_waitForMcuRestart")}</strong>
    {/if}
  {/if}
</div>

<style lang="scss">
  @use "../styles/app.scss" as *;

  .loader {
    display: inline-block;
    vertical-align: baseline;
    border: 6px solid #f3f3f3;
    border-radius: 50%;
    border-top: 6px solid #3498db;
    width: 12px;
    height: 12px;
    -webkit-animation: spin 2s linear infinite; /* Safari */
    animation: spin 2s linear infinite;
  }

  /* Safari */
  @-webkit-keyframes spin {
    0% {
      -webkit-transform: rotate(0deg);
    }
    100% {
      -webkit-transform: rotate(360deg);
    }
  }

  @keyframes spin {
    0% {
      transform: rotate(0deg);
    }
    100% {
      transform: rotate(360deg);
    }
  }
</style>
