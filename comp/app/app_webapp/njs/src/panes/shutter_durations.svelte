<script>
  "use strict";
  import { _ } from "services/i18n";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterPrefs from "app/shutter_prefs.svelte";
</script>

<div class="main-area">
  <div class="mt-4 p-2">
    <ShutterGM hideGroups="true" />
  </div>
  <h4>{$_('app.dur.h')}</h4>
  <div class="area">
    <ShutterPrefs />
  </div>
</div>
