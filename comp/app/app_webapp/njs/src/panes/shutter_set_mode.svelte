<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import { GuiAcc } from "stores/app_state";
  import ShutterSetModeEnable from "app/shutter_set_mode_enable.svelte";
  import ShutterRegister from "app/shutter_register.svelte";


 


</script>

<div class="main-area">
  <h4>{$_("app.setMode.h_set_mode_enable")}</h4>
  <div class="area">
    <ShutterSetModeEnable />
  </div>

  <h4>{$_("app.setMode.register_unregister")}</h4>
  <div class="area">
    <ShutterRegister />
  </div>
</div>
