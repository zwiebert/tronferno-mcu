<script>
  import { _ } from "services/i18n";
  import * as misc from "app/misc.js";
  import NavTabs from "app/nav_tabs.svelte";
  import { TabIdx } from "stores/app_state.js";

  import PaneShutterControl from "panes/shutter_control.svelte";
  import PaneShuttersPct from "panes/shutters_pct.svelte";
  import PaneShutterAuto from "panes/shutter_auto.svelte";
  import PaneShutterAlias from "panes/shutter_alias.svelte";
  import PaneShutterDurations from "panes/shutter_durations.svelte";
  import PaneShutterName from "panes/shutter_name.svelte";
  import PaneFirmwareEsp32 from "panes/firmware_esp32.svelte";
  import PaneWappSettings from "panes/wapp_settings.svelte";
  import PaneMcuSettings from "panes/mcu_settings.svelte";
  import Pane2411 from "panes/2411.svelte";
  import PaneShutterSetMode from "panes/shutter_set_mode.svelte";

  import PaneDeveloper from "panes/developer.svelte";

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
</script>

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
    <NavTabs
      nav_tabs={[$_('app.navTab.main.move'), '2411', $_('app.navTab.main.percent'), $_('app.navTab.main.auto'), $_('app.navTab.main.config'), ...(!misc.NODE_ENV_DEV ? [] : ['Test'])]}
      name="main" />
  </div>
  {#if !tabIdxMain}
    <PaneShutterControl />
  {:else if tabIdxMain === 1}
    <Pane2411 />
  {:else if tabIdxMain === 2}
    <PaneShuttersPct />
  {:else if tabIdxMain === 3}
    <PaneShutterAuto />
  {:else if tabIdxMain === 4}
    <div class="navtab-sub">
      <NavTabs
        nav_tabs={[$_('app.navTab.cfg.mcu'), $_('app.navTab.cfg.aliases'), $_('app.navTab.cfg.durations'), $_('app.navTab.cfg.name'), $_('app.navTab.cfg.set_mode'), $_('app.navTab.cfg.app'), $_('app.navTab.main.firmware')]}
        name="settings" />
    </div>
    {#if !tabIdxSettings}
      <PaneMcuSettings />
    {:else if tabIdxSettings === 1}
      <PaneShutterAlias />
    {:else if tabIdxSettings === 2}
      <PaneShutterDurations />
    {:else if tabIdxSettings === 3}
      <PaneShutterName />
    {:else if tabIdxSettings === 4}
      <PaneShutterSetMode />
    {:else if tabIdxSettings === 5}
      <PaneWappSettings />
      {:else if tabIdxSettings === 6}
      <PaneFirmwareEsp32 />
    {/if}
  {:else if !misc.DISTRO && tabIdxMain === 5}
    <PaneDeveloper />
  {/if}
</div>


<p class="help-text-v1 help-text-v0">no_purge_css</p>