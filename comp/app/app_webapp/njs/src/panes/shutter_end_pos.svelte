<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import ShutterGM from "app/shutter_gm.svelte";
  import SepEnable from "app/sep_enable.svelte";
</script>

<h4 class="text-center" use:tippy={{ content: $_("app.navTab.cfg.endpos.tt.header") }}>{$_("app.navTab.cfg.endpos.header")}</h4>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM group={false} />
  </div>

  <SepEnable />
</div>
