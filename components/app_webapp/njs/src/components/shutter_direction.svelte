<script>
  "use strict";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import * as httpFetch from "../app/fetch.js";
  import { GMH, RadioCode, RadioCodeEnabled, Address } from "../store/curr_shutter.js";
  import FaArrowsAltV from "svelte-icons/fa/FaArrowsAltV.svelte";

  let target = $derived($RadioCodeEnabled ? $RadioCode : $GMH);

  function postShutterCommand(cmd) {
    if (!$RadioCodeEnabled) return httpFetch.http_postShutterCommand(cmd);

    const mc = $RadioCode;
    if (!mc) return;

    let req = { cmd: { ...$Address, c: cmd } };
    httpFetch.http_postCommand(req);
  }
  function hClick_RotDir() {
    postShutterCommand("rot-dir");
  }

  function hClick_Up() {
    postShutterCommand("up");
    setTimeout(() => {
      postShutterCommand("stop");
    }, 1000);
  }

  function hClick_Down() {
    postShutterCommand("down");
    setTimeout(() => {
      postShutterCommand("stop");
    }, 1000);
  }
</script>

<div class="inline-block">
  <div class="flex flex-row items-center">
    <button class="sb w-16 h-16 text-lg rounded-l-full rounded-r-full" type="button" onclick={hClick_Down} use:tippy={{ content: $_("app.rotDir.tt.test_down") }}>
      {$_("app.rotDir.test_down")}
    </button>

    <button class="sb m-2 w-32 h-16 text-lg rounded-l-full rounded-r-full" type="button" onclick={hClick_RotDir} use:tippy={{ content: $_("app.rotDir.tt.toggle") }}>
      <div class="flex flex-row">
        <div class="h-4 w-4"><FaArrowsAltV /></div>
        {$_("app.rotDir.toggle")}
        <div class="h-4 w-4"><FaArrowsAltV /></div>
      </div>
    </button>

    <button class="sb w-16 h-16 text-lg rounded-l-full rounded-r-full" type="button" onclick={hClick_Up} use:tippy={{ content: $_("app.rotDir.tt.test_up") }}>
      {$_("app.rotDir.test_up")}
    </button>
  </div>
</div>

