<script>
  "use strict";
  import { _ } from "services/i18n";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterPrefs from "app/shutter_prefs.svelte";
  import tippy from "sveltejs-tippy";
</script>

<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("help.hint_shutterDurations") }}>{$_("app.dur.h")}</h4>
  
  <div class="mt-4 p-2">
    <ShutterGM />
  </div>

  <div class="area">
    <ShutterPrefs />
  </div>
</div>
