<script>
  "use strict";
  import ShutterGM from "../shutter_gm.svelte";
  import * as httpFetch from "../fetch.js";
  import ShutterPrefs from "../shutter_prefs.svelte";
</script>

<div class="mt-4 p-2">
  <ShutterGM
    gmc_fetch_mask={httpFetch.FETCH_ALIASES | httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME} />
</div>
<div class="area">
  <ShutterPrefs />
</div>