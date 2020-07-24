<script>
  "use strict";
  import ShutterGM from "../shutter_gm.svelte";
  import ShutterMove from "../shutter_move.svelte";
  import ShutterSun from "../shutter_sun.svelte";
  import ShutterPct from "../shutter_pct.svelte";
  import * as httpFetch from "../fetch.js";
</script>

<div class="area">
  <div class="mt-4 p-2">
    <ShutterGM
      gmc_fetch_mask={httpFetch.FETCH_AUTO | httpFetch.FETCH_POS | httpFetch.FETCH_SHUTTER_NAME} />
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
