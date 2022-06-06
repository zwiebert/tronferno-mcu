<script>
  "use strict";

  import { SepModeEnabled } from "stores/app_state";
</script>

<button type="button" on:click={() => {$SepModeEnabled = true;}}>Enter SepMode</button>