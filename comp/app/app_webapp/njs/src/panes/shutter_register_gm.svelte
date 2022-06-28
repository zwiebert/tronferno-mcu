<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import ShutterRegisterGm from "app/shutter_register_gm.svelte";
</script>

<h4 class="text-center" use:tippy={{ content: $_("app.navTab.cfg.set_mode.register_gm.tt.header") }}>{$_("app.navTab.cfg.set_mode.register_gm.header")}</h4>

<div class="area">
  <ShutterRegisterGm />
</div>
