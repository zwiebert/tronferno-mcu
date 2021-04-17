<script>
  "use strict";
  import ShutterSelectGM from "components/shutter_select_gm.svelte";
  import { _ } from "services/i18n";
  import { G, M0, Name } from "stores/curr_shutter.js";
  import { SetModeSrcAddress, SetModeSrcRadio, SetModeSrcMotorCode, SetModeSrcProgress } from "stores/shutter_set_mode.js";
  import * as httpFetch from "app/fetch.js";

  $: name = $Name || "";

  function shn_fromHtml_toMcu(val) {
    let tfmcu = { to: "tfmcu", shpref: { g: $G, m: $M0, "tag.NAME": val } };
    httpFetch.http_postCommand(tfmcu);
  }

  function hChange_Name() {
    shn_fromHtml_toMcu(name);
  }

  function onClick_SetByGM() {
    let tfmcu = { to: "tfmcu", cmd: { g: $G, m: $M0, c: "set" } };
    httpFetch.http_postCommand(tfmcu);
    enterSetMode();
  }

  function onClick_SetByAddr() {
    let tfmcu = { to: "tfmcu", cmd: { a: $SetModeSrcAddress, c: "set" } };
    httpFetch.http_postCommand(tfmcu);
    enterSetMode();
  }
  function onClick_SetByMotorCode() {
    let tfmcu = { to: "tfmcu", cmd: { a: "9"+$SetModeSrcMotorCode, c: "set" } };
    httpFetch.http_postCommand(tfmcu);
    enterSetMode();
  }

  function enterSetMode() {
    $SetModeSrcProgress = 60;
    let iv = setInterval(()=>{
      if (0 >= $SetModeSrcProgress--) {
        clearInterval(iv);
      }

    },1000);
  }
</script>

<style type="text/scss">
</style>

<p>
  <label> <input type="radio" bind:group={$SetModeSrcRadio} value={0} /> {$_('app.setMode.gm_address')}</label>
  <label> <input type="radio" bind:group={$SetModeSrcRadio} value={2} /> {$_('app.setMode.motor_code')}</label>
  <label><input type="radio" bind:group={$SetModeSrcRadio} value={1} /> {$_('app.setMode.sender_address')} </label>
  <label><input type="radio" bind:group={$SetModeSrcRadio} value={3} /> {$_('app.setMode.set_button_radio')} </label>
</p>

{#if $SetModeSrcRadio === 0}
  <label>{$_('app.setMode.gm_address')}
    <ShutterSelectGM />
  </label>
  <button on:click={onClick_SetByGM}>{$_('app.setMode.set_mode')}</button>
{:else if $SetModeSrcRadio === 1}
  <label>{$_('app.setMode.sender_address')} <input type="text" class="w-20" bind:value={$SetModeSrcAddress} on:change={hChange_Name} /> </label>
  <button on:click={onClick_SetByAddr}>{$_('app.setMode.set_mode')}</button>
{:else if $SetModeSrcRadio === 2}
  <label>{$_('app.setMode.motor_code')} <input type="text" class="w-20" bind:value={$SetModeSrcMotorCode} on:change={hChange_Name} /> </label>
  <button on:click={onClick_SetByMotorCode}>{$_('app.setMode.set_mode')}</button>
  {:else if $SetModeSrcRadio === 3}
  <button on:click={enterSetMode}>{$_('app.setMode.set_button')}</button>
{/if}

{#if $SetModeSrcProgress > 0}
  <progress value={$SetModeSrcProgress} max={60} />
{/if}

<div class="text-center"><br /></div>
