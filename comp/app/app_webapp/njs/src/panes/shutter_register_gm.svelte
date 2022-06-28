<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import NavTabs from "app/nav_tabs.svelte";
  import { GuiAcc } from "stores/app_state";
  import ShutterSetModeEnable from "app/shutter_set_mode_enable.svelte";
  import ShutterRegister from "app/shutter_register.svelte";

</script>



<h4 class="text-center" use:tippy={{ content: $_("app.navTab.cfg.set_mode.register_gm.tt.header") }}>{$_("app.navTab.cfg.set_mode.register_gm.header")}</h4>

<div class="area">
  <h4>{$_("app.setMode.register_unregister")}</h4>
  <ShutterRegister />
</div>
