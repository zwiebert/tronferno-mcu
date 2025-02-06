<script>
  "use strict";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { SetModeDstG, SetModeDstM } from "../store/shutter_set_mode.js";
  import * as httpFetch from "../app/fetch.js";

  let transmitCountDown = $state(0);
  

  function onClick_registerCu() {
    let tfmcu = { to: "tfmcu", auto: { g: $SetModeDstG, m: $SetModeDstM, f: "k" } };
    httpFetch.http_postCommand(tfmcu);

    // disable button for 5 seconds while data is being sent to shutter
    // motor by RF
    transmitCountDown = 5;
    const iv = setInterval(() => {
      if (--transmitCountDown <= 0) clearInterval(iv);
    }, 1000);
  }
</script>

<label>G: <input type="number" bind:value={$SetModeDstG} min="1" max="7" step="1" class="w-10" /></label>
<label>E: <input type="number" bind:value={$SetModeDstM} min="1" max="7" step="1" class="w-10" /></label>

<button
  id="asvb"
  class="sb rounded-l-full rounded-r-full"
  type="button"
  use:tippy={{ content: $_("app.setMode.tt.register_cu_bt") }}
  disabled={transmitCountDown > 0}
  onclick={onClick_registerCu}>{transmitCountDown > 0 ? "-" + transmitCountDown + "-" : $_("app.auto.sendData")}</button
>

