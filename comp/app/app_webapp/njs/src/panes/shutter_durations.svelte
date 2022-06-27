<script>
  "use strict";
  import { _ } from "services/i18n";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterPrefs from "app/shutter_prefs.svelte";
  import tippy from "sveltejs-tippy";
</script>

<h4 class="text-center" use:tippy={{ content: $_("app.navTab.cfg.durations.tt.header") }}>{$_("app.navTab.cfg.durations.header")}</h4>

<div class="area">
  <div class="mt-4 p-2">
    <ShutterGM group={false} />
  </div>

  <ShutterPrefs />
</div>