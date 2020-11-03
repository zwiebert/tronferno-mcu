<script>
  "use strict";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterMove from "app/shutter_move.svelte";
  import ShutterSun from "app/shutter_sun.svelte";
  import ShutterPct from "app/shutter_pct.svelte";
  import * as httpFetch from "app/fetch.js";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM />
  </div>

  <div class="area">
    <ShutterMove />
  </div>

  <div class="area">
    <ShutterPct />
  </div>

  <div class="area">
    <ShutterSun />
  </div>
</div>
