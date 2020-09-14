<script>
  "use strict";
  import ShutterGM from "../shutter_gm.svelte";
  import * as httpFetch from "../fetch.js";
  import ShutterAlias from "../shutter_alias.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM
      gmc_fetch_mask={httpFetch.FETCH_ALIASES | httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME}
      hideGroups="true" />
  </div>
  <div class="area">
    <ShutterAlias />
  </div>
</div>
