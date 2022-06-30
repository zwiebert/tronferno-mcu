<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import NavTabs from "app/nav_tabs.svelte";
  import { GuiAcc } from "stores/app_state";
  import ShutterSetModeEnable from "app/shutter_set_mode_enable.svelte";
</script>

<h4 class="text-center" use:tippy={{ content: $_("panes.activate.tt.header") }}>{$_("panes.activate.header")}</h4>

<div class="area">
  <div class="mt-4 p-2">
    <ShutterSetModeEnable />
  </div>
</div>
