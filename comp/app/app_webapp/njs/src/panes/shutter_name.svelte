<script>
  "use strict";
  import { _ } from "services/i18n";
  import ShutterGM from "app/shutter_gm.svelte";
  import ShutterName from "app/shutter_name.svelte";
  import { GuiAcc } from "stores/app_state";
  import tippy from "sveltejs-tippy";
</script>

{#if $GuiAcc.edit_shutter_names}
  <h4 class="text-center" use:tippy={{ content: $_("panes.rx_names.tt.header") }}>{$_("panes.rx_names.header")}</h4>

  <div class="area">
    <div class="mt-4 p-2">
      <ShutterGM group={false} />
    </div>

    <ShutterName />
  </div>
{/if}
