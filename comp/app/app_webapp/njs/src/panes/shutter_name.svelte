<script>
  "use strict";
  import ShutterGM from "../shutter_gm.svelte";
  import * as httpFetch from "../fetch.js";
  import ShutterName from "../shutter_name.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM />
  </div>
  <div class="area">
    <ShutterName />
  </div>
</div>
