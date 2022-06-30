<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import ShutterRegisterTx from "app/shutter_register_tx.svelte";
</script>

<h4 class="text-center" use:tippy={{ content: $_("panes.register_tx.tt.header") }}>{$_("panes.register_tx.header")}</h4>

<div class="area">
  <ShutterRegisterTx />
</div>
