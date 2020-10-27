<script>
  "use strict";
  import { _ } from "../services/i18n";
  import { SetMode_isInSetMode, SetModeDstAddress, SetModeDstG, SetModeDstM, SetModeDstRadio } from "../store/shutter_set_mode.js";
  import ShutterSetModeEnable from "../shutter_set_mode_enable.svelte";
  import * as httpFetch from "../fetch.js";

  function onClick_registerCu() {
    let tfmcu = { to: "tfmcu", auto: { g: $SetModeDstG, m: $SetModeDstM, f: "k" } };
    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
    $SetMode_isInSetMode = false;
  }

  function onClick_registerSender() {
    let tfmcu = { to: "tfmcu", cmd: { a: $SetModeDstAddress, c: "stop" } };
    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
    $SetMode_isInSetMode = false;
  }
</script>

<div class="main-area">
  <h4>{$_('app.setMode.h_set_mode_enable')}</h4>
  <div class="area">
    <ShutterSetModeEnable />
  </div>

  <h4>{$_('app.setMode.register_unregister')}</h4>
  <div class="area">
    <p>
      <label> <input type="radio" bind:group={$SetModeDstRadio} value={0} /> {$_('app.setMode.register_cu')}</label>
      <label> <input type="radio" bind:group={$SetModeDstRadio} value={1} /> {$_('app.setMode.register_sender')}</label>
    </p>

    {#if $SetModeDstRadio === 0}
      <label>G: <input type="number" value="1" min="1" max="7" step="1" class="w-10" /></label>
      <label>E: <input type="number" value="1" min="1" max="7" step="1" class="w-10" /></label>
      <button disabled={!$SetMode_isInSetMode} on:click={onClick_registerCu}>{$_('app.setMode.register_unregister')}</button>
    {:else if $SetModeDstRadio === 1}
      <label>{$_('app.setMode.sender_address')} <input bind:value={$SetModeDstAddress} class="w-24" type="text" /></label>
      <button disabled={!$SetMode_isInSetMode} on:click={onClick_registerSender}>{$_('app.setMode.register_unregister')}</button>
    {/if}
  </div>
</div>
