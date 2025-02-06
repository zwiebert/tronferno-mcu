<script>
  "use strict";
  import ShutterSelectGM from "../components/shutter_select_gm.svelte";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { GuiAcc } from "../store/app_state";
  import { Address, Name } from "../store/curr_shutter.js";
  import { SetModeSrcRadio, SetModeSrcProgress } from "../store/shutter_set_mode.js";
  import IdSelector from "../app/id_selector.svelte";
  import { SelectedId, SelectedIdIsValid } from "../store/id.js";
  import * as httpFetch from "../app/fetch.js";

  let name = $derived($Name || "");

  function onClick_SetByGM() {
    let tfmcu = { to: "tfmcu", cmd: { ...$Address, c: "set" } };
    httpFetch.http_postCommand(tfmcu);
    enterSetMode();
  }

  function onClick_SetByAddr() {
    let tfmcu = { to: "tfmcu", cmd: { a: $SelectedId, c: "set" } };
    httpFetch.http_postCommand(tfmcu);
    enterSetMode();
  }

  function enterSetMode() {
    $SetModeSrcProgress = 60;
    let iv = setInterval(() => {
      if (0 >= $SetModeSrcProgress--) {
        clearInterval(iv);
      }
    }, 1000);
  }
</script>

<p>
  <label> <input type="radio" bind:group={$SetModeSrcRadio} value={0} /> {$_("app.setMode.gm_address")}</label>
  {#if $GuiAcc.set_func_by_transmitter_id}
    <label><input type="radio" bind:group={$SetModeSrcRadio} value={1} /> {$_("app.setMode.sender_address")} </label>
  {/if}
  <label><input type="radio" bind:group={$SetModeSrcRadio} value={3} /> {$_("app.setMode.set_button_radio")} </label>
</p>

{#if $SetModeSrcRadio === 0}
  <ShutterSelectGM />
  <button class="sb text-sm rounded-full" onclick={onClick_SetByGM} use:tippy={{ content: $_("app.setMode.tt.set_mode") }}>
    {$_("app.setMode.set_mode")}
  </button>
{:else if $SetModeSrcRadio === 1}
  <IdSelector />
  <button class="sb text-sm rounded-full" onclick={onClick_SetByAddr} disabled={!$SelectedIdIsValid}> {$_("app.setMode.set_mode")} </button>
{:else if $SetModeSrcRadio === 3}
  <button class="sb" onclick={enterSetMode} use:tippy={{ content: $_("app.setMode.tt.set_button") }}> {$_("app.setMode.set_button")} </button>
{/if}

{#if $SetModeSrcProgress > 0}
  <br />
  <progress value={$SetModeSrcProgress} max={60}></progress>
{/if}

<div class="text-center"><br /></div>

<style lang="scss">
</style>
