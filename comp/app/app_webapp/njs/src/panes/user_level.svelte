<script>
  "use strict";

  import { SepMode } from "stores/sep";
  import { GuiUserLevel } from "stores/app_state";
  import * as misc from "app/misc.js";
</script>

<div class="main-area text-center">
  <h4>GUI access level (experimental)</h4>
  <select bind:value={$GuiUserLevel}>
    {#if misc.NODE_ENV_DEV}
      <option value={-1}>DEBUG</option>
    {/if}
    <option value={0}>expert</option>
    <option value={10}>admin</option>
    <option value={20}>user</option>
    <option value={30}>kid</option>
  </select>
</div>
