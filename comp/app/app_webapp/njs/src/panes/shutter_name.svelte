<script>
  "use strict";
  import { _ } from "services/i18n";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterName from "app/shutter_name.svelte";
  import ShutterDirection from "components/shutter_direction.svelte";
  import SepEnable from "app/sep_enable.svelte";
  import { GuiAcc } from "stores/app_state";
  import tippy from "sveltejs-tippy";
</script>

<div class="text-center main-area">
  <h4 class="text-center" use:tippy={{ content: $_("help.hint_shutterMisc") }}>{$_("app.navTab.cfg.name")}</h4>

  <div class="mt-4 p-2">
    <ShutterGM />
  </div>

  {#if $GuiAcc.edit_shutter_names}
    <div class="area">
      <h5>{$_("app.msg_enterNewName")}</h5>
      <ShutterName />
    </div>
  {/if}
</div>
