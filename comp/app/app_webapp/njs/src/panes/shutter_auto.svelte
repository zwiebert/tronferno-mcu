<script>
  "use strict";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterAuto from "app/shutter_auto.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM radio={false} />
  </div>
  <div class="area">
    <ShutterAuto />
  </div>
</div>
