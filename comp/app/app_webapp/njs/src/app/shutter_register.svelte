<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import { GuiAcc } from "stores/app_state";
  import { SetModeSrcProgress, SetModeDstG, SetModeDstM, SetModeDstRadio } from "stores/shutter_set_mode.js";
  import IdSelector from "app/id_selector.svelte";
  import { SelectedId, SelectedIdIsValid } from "stores/id.js";
  import * as httpFetch from "app/fetch.js";

  $: SetModeAutoProgress = 0;

  function startAutoProgress() {
    SetModeAutoProgress = 5;
    let iv = setInterval(() => {
      if (0 >= SetModeAutoProgress--) {
        clearInterval(iv);
      }
    }, 1000);
  }

  function onClick_registerCu() {
    let tfmcu = { to: "tfmcu", auto: { g: $SetModeDstG, m: $SetModeDstM, f: "k" } };
    httpFetch.http_postCommand(tfmcu);
    startAutoProgress();

    setTimeout(() => {
      $SetModeSrcProgress = 0;
    }, 5000);
  }

  function onClick_registerSender() {
    let tfmcu = { to: "tfmcu", cmd: { a: $SelectedId, c: "stop" } };
    httpFetch.http_postCommand(tfmcu);
    $SetModeSrcProgress = 0;
  }
</script>

<p>
  <label use:tippy={{ content: $_("app.setMode.tt.register_cu") }}>
    <input type="radio" bind:group={$SetModeDstRadio} value={0} /> {$_("app.setMode.register_cu")}</label
  >
  {#if $GuiAcc.register_by_transmitter_id}
    <label use:tippy={{ content: $_("app.setMode.tt.register_sender") }}>
      <input type="radio" bind:group={$SetModeDstRadio} value={1} /> {$_("app.setMode.register_sender")}</label
    >
  {/if}
</p>

{#if $SetModeDstRadio === 0}
  <label>G: <input type="number" bind:value={$SetModeDstG} min="1" max="7" step="1" class="w-10" /></label>
  <label>E: <input type="number" bind:value={$SetModeDstM} min="1" max="7" step="1" class="w-10" /></label>
  <button disabled={$SetModeSrcProgress <= 0} on:click={onClick_registerCu} use:tippy={{ content: $_("app.setMode.tt.register_cu_bt") }}
    >{$_("app.setMode.register_cu_bt")}</button
  >
{:else if $SetModeDstRadio === 1}
  <IdSelector />
  <button disabled={$SetModeSrcProgress <= 0 || !$SelectedIdIsValid} on:click={onClick_registerSender}>{$_("app.setMode.register_unregister")}</button>
{/if}

{#if SetModeAutoProgress > 0}
  <progress value={SetModeAutoProgress} max={5} />
{/if}
