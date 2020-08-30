<script>
  "use strict";
  import ShutterGM from "../shutter_gm.svelte";
  import * as httpFetch from "../fetch.js";
  import ShutterAuto from "../shutter_auto.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM
      gmc_fetch_mask={httpFetch.FETCH_AUTO | httpFetch.FETCH_SHUTTER_NAME} />
  </div>
  <div class="area">
    <ShutterAuto />
  </div>
</div>
