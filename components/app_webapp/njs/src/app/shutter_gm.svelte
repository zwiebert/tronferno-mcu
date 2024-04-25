<script>
  "use strict";
  import ShutterSelectGM from "../components/shutter_select_gm.svelte";
  export let radio = true;
  export let groups = true;
</script>

<div class="text-center">
  <ShutterSelectGM {radio} {groups} />
</div>
