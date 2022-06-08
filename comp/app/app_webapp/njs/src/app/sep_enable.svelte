<script>
  "use strict";

  import { SepMode } from "stores/app_state";

  function create_auth_key() {
    return Math.floor(Math.random() * 10000) + 1;
  }
</script>

<button
  type="button"
  on:click={() => {
    $SepMode.auth_key = create_auth_key();
    $SepMode.enabled = true;
  }}>Enter SepMode</button
>
