<script>
  "use strict";
  import { Verbose } from "./store/app_state.js";
</script>

<label>Verbosity:</label>
<select bind:value={$Verbose}>
  <option value="0">0</option>
  <option value="1">1</option>
</select>
