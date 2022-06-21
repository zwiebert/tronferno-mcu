<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import { GuiAcc, TabIdx } from "stores/app_state";
  import NavTabs from "app/nav_tabs.svelte";
  import ShutterGM from "app/shutter_gm.svelte";
  import PaneShutterAuto from "panes/shutter_auto.svelte";

  import PaneShutterName from "panes/shutter_name.svelte";
  import PaneShutterSetMode from "panes/shutter_set_mode.svelte";
  import ShutterDirection from "components/shutter_direction.svelte";
  import SepEnable from "app/sep_enable.svelte";
  import PaneShutterDurations from "panes/shutter_durations.svelte";

  $: tabIdxRx = $TabIdx["receiver"] || 0;
</script>

<div class="navtab-sub2">
  <NavTabs
    nav_tabs={[
      ...($GuiAcc.edit_shutter_names ? [{ name: $_("app.navTab.cfg.name"), idx: 0 }] : []),
      ...($GuiAcc.set_mode ? [{ name: $_("app.navTab.cfg.set_mode"), idx: 1 }] : []),
      ...($GuiAcc.program_shutter_rotation_direction ? [{ name: $_("app.navTab.cfg.rot_dir"), idx: 2 }] : []),
      ...($GuiAcc.shutter_sep ? [{ name: $_("app.navTab.cfg.endpos"), idx: 3 }] : []),
      ...($GuiAcc.edit_shutter_durations ? [{ name: $_("app.navTab.positions.durations"), idx: 4 }] : []),

      //...($GuiAcc.shutter_auto ? [{ name: $_("app.navTab.main.auto"), idx: 0 }] : []),
    ]}
    name="receiver"
  />
</div>
<div class="text-center main-area">
  {#if tabIdxRx === 5 && $GuiAcc.shutter_auto}
    <PaneShutterAuto />
  {:else if tabIdxRx === 2 && $GuiAcc.program_shutter_rotation_direction}
    <div class="mt-4 p-2">
      <ShutterGM />
    </div>
    <div class="area">
      <h5>{$_("app.hdr_motorRotDir")}</h5>
      <ShutterDirection />
    </div>
  {:else if tabIdxRx === 3 && $GuiAcc.shutter_sep}
    <div class="mt-4 p-2">
      <ShutterGM />
    </div>

    <div class="area">
      <h5>{$_("app.sep.header")}</h5>
      <SepEnable />
    </div>
  {:else if tabIdxRx === 1 && $GuiAcc.set_mode}
    <PaneShutterSetMode />
  {:else if tabIdxRx === 0 && $GuiAcc.edit_shutter_names}
    <PaneShutterName />
  {:else if tabIdxRx === 4 && $GuiAcc.edit_shutter_durations}
    <PaneShutterDurations />
  {/if}
</div>
