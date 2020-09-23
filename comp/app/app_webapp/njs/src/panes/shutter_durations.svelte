<script>
  "use strict";
  import ShutterGM from "../shutter_gm.svelte";
  import * as httpFetch from "../fetch.js";
  import ShutterPrefs from "../shutter_prefs.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM hideGroups="true" />
  </div>
  <div class="area">
    <ShutterPrefs />
  </div>
</div>
