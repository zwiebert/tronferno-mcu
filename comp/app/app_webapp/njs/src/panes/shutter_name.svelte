<script>
  "use strict";
  import { _ } from "services/i18n";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterName from "app/shutter_name.svelte";
  import ShutterDirection from "components/shutter_direction.svelte";
  import SepEnable from "app/sep_enable.svelte";
  import tippy from "sveltejs-tippy";

</script>

<div class="text-center main-area">
  <h4 class="text-center" use:tippy={{ content: $_("help.hint_shutterMisc") }}>{$_('app.navTab.cfg.name')}</h4>

  <div class="mt-4 p-2">
    <ShutterGM />
  </div>

  <div class="area">

  <h5>{$_('app.msg_enterNewName')}</h5>
    <ShutterName />
  </div>

  <div class="area">
    <h5>{$_('app.hdr_motorRotDir')}</h5>
    <ShutterDirection />
  </div>

  <div class="area">
    <h5>{$_('app.sep_enable')}</h5>
    <SepEnable />
  </div>

</div>
