<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterDirection from "components/shutter_direction.svelte";
</script>

<h4 class="text-center" use:tippy={{ content: $_("app.navTab.cfg.rot_dir.tt.header") }}>{$_("app.navTab.cfg.rot_dir.header")}</h4>

<div class="area">
  <div class="mt-4 p-2">
    <ShutterGM group={false} />
  </div>

  <ShutterDirection />
</div>
