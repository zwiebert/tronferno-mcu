<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import ShutterRegisterTx from "app/shutter_register_tx.svelte";
</script>

<h4 class="text-center" use:tippy={{ content: $_("app.navTab.cfg.set_mode.register_tx.tt.header") }}>{$_("app.navTab.cfg.set_mode.register_tx.header")}</h4>

<div class="area">
  <ShutterRegisterTx />
</div>
