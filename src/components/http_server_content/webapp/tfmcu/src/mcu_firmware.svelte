<script>
  "use strict";
  import { McuBootCount } from "./store/mcu_firmware.js";
  
  export let fwbtns = [];
  export let McuFwGitTags;
  import { onMount,onDestroy } from 'svelte';
  
  let on_destroy = [];
onMount(() => {
     httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION | httpFetch.FETCH_GIT_TAGS);
  });
onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
});


</script>

<script context="module">
  import * as misc from "./misc.js";
  import * as appDebug from "./app_debug.js";
  import * as httpFetch from "./fetch.js";

  let netota_intervalID = 0;
  let netota_progressCounter = 0;
  let netota_isInProgress = false;
  

  function netota_FetchFeedback() {
    let netmcu = { to: "tfmcu" };
    netmcu.mcu = {
      mcuFirmware: "?",
    };
    let url = "/cmd.json";
    appDebug.dbLog("url: " + url);
    httpFetch.http_postRequest(url, netmcu);
  }

  export function netFirmwareOTA(ota_name) {
    if (netota_isInProgress) return;
    let netmcu = { to: "tfmcu" };
    netmcu.mcu = {
      mcuFirmware: ota_name,
    };
    let url = "/cmd.json";
    appDebug.dbLog("url: " + url);
    httpFetch.http_postRequest(url, netmcu);
    document.getElementById("netota_progress_div").innerHTML =
      "<strong>Firmware is updating...<br></strong>" +
      '<progress id="netota_progress_bar" value="0" max="30">70 %</progress>';
    netota_intervalID = setInterval(netota_FetchFeedback, 1000);
    netota_isInProgress = true;
    document.getElementById("netota_controls").style.display = "none";
  }

  export function netota_handle_otaState(ota_state) {
    let e = document.getElementById("netota_progress_div");
    switch (ota_state) {
      case 0: // none
        break;
      case 1: // run
        document.getElementById(
          "netota_progress_bar"
        ).value = (++netota_progressCounter).toString();
        break;
      case 2: // fail
        e.innerHTML += "<br><strong>Update failed<br><br></strong>";
        break;
      case 3: // done
        document.getElementById(
          "netota_progress_bar"
        ).value = document.getElementById("netota_progress_bar").max;
        e.innerHTML +=
          '<br><strong>Update succeeded <button id="ota_reboot" type="button">Reboot MCU</button><br><br></strong>';
        document.getElementById("ota_reboot").onclick = () =>
          misc.req_mcuRestart();
        break;
    }
    if (netota_isInProgress && ota_state !== 1) {
      clearInterval(netota_intervalID);
      netota_progressCounter = 0;
      netota_isInProgress = false;
      document.getElementById("netota_controls").style.display = "";
    }
  }
</script>

<div id="id-fwDiv">
  <h4>OTA Firmware Update</h4>
  <div id="netota_controls">
    {#each fwbtns as bt}
      {#if !bt.input}
        <label>
          <button type="button" on:click={() => netFirmwareOTA(bt.ota_name)}>
            Do flash {bt.name}
          </button>
        </label>
        <br />
        <br />
      {:else if bt.input === 'input'}
        <label>
          <button
            type="button"
            on:click={() => netFirmwareOTA(document.getElementById(bt.ota_name).value)}>
            Do flash {bt.name}
          </button>
          <input type="text" id={bt.ota_name} value={bt.input_value} />
        </label>
        <br />
        <br />
      {/if}
    {/each}

    {#if McuFwGitTags}
      <!-- svelte-ignore reactive-component -->
       <svelte:component this={McuFwGitTags}/>
    {/if}
  </div>
 
  <div id="netota_progress_div" />
  <div id="netota_restart_div" />

  <h4>Firmware Info</h4>
  <ul>
    <li>
      MCU type:
      <span id="id_chip" />
    </li>
    <li>
      firmware version:
      <span id="id_firmware" />
    </li>
    <li>
      firmware build date:
      <span id="id_buildTime" />
    </li>
    <li>
      <label>
        boot-count:
        <span id="id-bootCount">{$McuBootCount}</span>
      </label>
    </li>
  </ul>
</div>
