<script>
  "use strict";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import ShutterRegisterGm from "../app/shutter_register_gm.svelte";
</script>

<h4 class="text-center" use:tippy={{ content: $_("panes.register_gm.tt.header") }}>{$_("panes.register_gm.header")}</h4>

<div class="area">
  <ShutterRegisterGm />
</div>
