<script>
  "use strict";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterName from "app/shutter_name.svelte";
  import ShutterDirection from "components/shutter_direction.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM />
  </div>
  <div class="area">
    <ShutterName />
  </div>

  <div class="area">
    <h5>Motor Rotation Direction</h5>
    <ShutterDirection />
  </div>
</div>
