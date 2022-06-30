<script>
  "use strict";

  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import { GuiUserLevel } from "stores/app_state";
  import * as misc from "app/misc.js";
</script>

<div class="main-area text-center">
  <h4 class="text-center" use:tippy={{ content: $_("panes.guiacc.tt.header") }}>{$_("panes.guiacc.header")}</h4>

  <div class="area text-center">
    <select bind:value={$GuiUserLevel} size="1">
      {#if misc.NODE_ENV_DEV}
        <option value={-1}> {$_("navTab.main.user_level.name.developer")}</option>
      {/if}
      <option value={0}>{$_("navTab.main.user_level.name.expert")}</option>
      <option value={10}>{$_("navTab.main.user_level.name.admin")}</option>
      <option value={20}>{$_("navTab.main.user_level.name.user")}</option>
      <option value={30}>{$_("navTab.main.user_level.name.kid")}</option>
    </select>
  </div>
</div>
