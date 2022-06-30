<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import { SetModeSrcProgress, SetModeDstG, SetModeDstM } from "stores/shutter_set_mode.js";
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
</script>

<label>G: <input type="number" bind:value={$SetModeDstG} min="1" max="7" step="1" class="w-10" /></label>
<label>E: <input type="number" bind:value={$SetModeDstM} min="1" max="7" step="1" class="w-10" /></label>
<button on:click={onClick_registerCu} use:tippy={{ content: $_("app.setMode.tt.register_cu_bt") }}>{$_("app.setMode.register_cu_bt")}</button>

{#if SetModeAutoProgress > 0}
  <progress value={SetModeAutoProgress} max={5} />
{/if}
