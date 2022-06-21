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

<div class="mt-4 p-2">
  <ShutterGM />
</div>

{#if $GuiAcc.edit_shutter_names}
  <div class="main-area">
    <h5>{$_("app.msg_enterNewName")}</h5>
    <ShutterName />
  </div>
{/if}
