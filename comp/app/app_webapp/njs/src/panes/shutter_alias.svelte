<script>
  "use strict";
  import { _ } from "services/i18n";
  import ShutterGM from "app/shutter_gm.svelte";
  import IdSelector from "app/id_selector.svelte";
  import ShutterAlias from "app/shutter_alias.svelte";
  import tippy from "sveltejs-tippy";
</script>

<div class="main-area text-center">
  <h4 class="text-center" use:tippy={{ content: $_("help.hint_shutterAlias") }}>Alias</h4>

  <div class="mt-4 p-2">
    <ShutterGM />
  </div>

<br />

  <div>
    <IdSelector />
  </div>

  <div>
    <ShutterAlias />
  </div>
</div>
