<script>
  "use strict";
  import ShutterGM from "app/shutter_gm.svelte";
  import * as httpFetch from "app/fetch.js";
  import ShutterAlias from "app/shutter_alias.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM hideGroups="true" />
  </div>
  <div class="area">
    <ShutterAlias />
  </div>
</div>
