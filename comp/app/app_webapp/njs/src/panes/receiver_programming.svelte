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
  import PaneShutterRegisterGm from "panes/shutter_register_gm.svelte";
  import PaneShutterRegisterTx from "panes/shutter_register_tx.svelte";

  import PaneShutterDirection from "panes/shutter_direction.svelte";
  import PaneShutterEndpos from "panes/shutter_end_pos.svelte";
  import PaneShutterDurations from "panes/shutter_durations.svelte";

  $: tabIdxRx = $TabIdx["receiver"] || 0;
  $: tabIdxSet = $TabIdx["set"] || 0;
</script>

<div class="navtab-sub2">
  <NavTabs
    nav_tabs={[
      ...($GuiAcc.edit_shutter_names ? [{ name: $_("navTab.cfg.receiver.names.tab"), idx: 0 }] : []),
      ...($GuiAcc.set_mode ? [{ name: $_("navTab.cfg.receiver.set_mode.tab"), idx: 1 }] : []),
      ...($GuiAcc.program_shutter_rotation_direction ? [{ name: $_("navTab.cfg.receiver.rot_dir.tab"), idx: 2 }] : []),
      ...($GuiAcc.shutter_sep ? [{ name: $_("navTab.cfg.receiver.endpos.tab"), idx: 3 }] : []),
      ...($GuiAcc.edit_shutter_durations ? [{ name: $_("navTab.cfg.receiver.durations.tab"), idx: 4 }] : []),

      //...($GuiAcc.shutter_auto ? [{ name: $_("navTab.main.auto"), idx: 0 }] : []),
    ]}
    name="receiver"
  />
</div>

{#if tabIdxRx === 1 && $GuiAcc.set_mode}
  <div class="navtab-sub3">
    <NavTabs
      nav_tabs={[
        ...($GuiAcc.set_mode ? [{ name: $_("navTab.cfg.receiver.set_mode.activate.tab"), idx: 0 }] : []),
        ...($GuiAcc.set_mode ? [{ name: $_("navTab.cfg.receiver.set_mode.register_gm.tab"), idx: 1 }] : []),
        ...($GuiAcc.set_mode ? [{ name: $_("navTab.cfg.receiver.set_mode.register_tx.tab"), idx: 2 }] : []),
      ]}
      name="set"
    />
  </div>
{/if}

<div class="text-center main-area">
  {#if tabIdxRx === 5 && $GuiAcc.shutter_auto}
    <PaneShutterAuto />
  {:else if tabIdxRx === 2 && $GuiAcc.program_shutter_rotation_direction}
    <PaneShutterDirection />
  {:else if tabIdxRx === 3 && $GuiAcc.shutter_sep}
    <PaneShutterEndpos />
  {:else if tabIdxRx === 1 && $GuiAcc.set_mode}
    {#if tabIdxSet === 0}
      <PaneShutterSetMode />
    {:else if tabIdxSet === 1}
      <PaneShutterRegisterGm />
    {:else if tabIdxSet === 2}
      <PaneShutterRegisterTx />
    {/if}
  {:else if tabIdxRx === 0 && $GuiAcc.edit_shutter_names}
    <PaneShutterName />
  {:else if tabIdxRx === 4 && $GuiAcc.edit_shutter_durations}
    <PaneShutterDurations />
  {/if}
</div>
