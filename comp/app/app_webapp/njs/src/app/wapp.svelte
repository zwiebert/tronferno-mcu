<script>
  import { _ } from "services/i18n";
  import * as misc from "app/misc.js";
  import { GuiAcc } from "stores/app_state";
  import NavTabs from "app/nav_tabs.svelte";
  import { TabIdx } from "stores/app_state.js";

  import PaneShutterControl from "panes/shutter_control.svelte";
  import PaneShuttersPct from "panes/shutters_pct.svelte";
  import PaneShutterAuto from "panes/shutter_auto.svelte";
  import PaneShutterAlias from "panes/shutter_alias.svelte";
  import PaneRepeater from "panes/repeater.svelte";
  import PaneShutterDurations from "panes/shutter_durations.svelte";
  import PaneShutterName from "panes/shutter_name.svelte";
  import PaneFirmwareEsp32 from "panes/firmware_esp32.svelte";
  import PaneMcuSettings from "panes/mcu_settings.svelte";
  import Pane2411 from "panes/2411.svelte";
  import PaneShutterSetMode from "panes/shutter_set_mode.svelte";
  import PaneAppLog from "panes/app_log.svelte";
  import PaneDeveloper from "panes/developer.svelte";
  import PaneUserLevel from "panes/user_level.svelte";
  import * as httpFetch from "app/fetch.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_NAMES);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
  $: tabIdxSender = $TabIdx["sender"] || 0;
  $: tabIdxPositions = $TabIdx["positions"] || 0;
</script>

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
    <NavTabs
      nav_tabs={[
        { name: $_("app.navTab.main.move"), idx: 0 },
        { name: "2411", idx: 1 },
        { name: $_("app.navTab.main.percent"), idx: 2 },
        ...($GuiAcc.shutter_auto ? [{ name: $_("app.navTab.main.auto"), idx: 3 }] : []),
        ...($GuiAcc.cfg ? [{ name: $_("app.navTab.main.config"), idx: 4 }] : []),
        { name: "UL", idx: 5 },
        ...($GuiAcc.debug ? [{ name: "Test", idx: 6 }] : []),
      ]}
      name="main"
    />
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
        nav_tabs={[
          { name: $_("app.navTab.cfg.mcu"), idx: 0 },
          { name: $_("app.navTab.cfg.sender"), idx: 1 },
          { name: $_("app.navTab.cfg.positions"), idx: 2 },
          { name: $_("app.navTab.cfg.name"), idx: 3 },
          { name: $_("app.navTab.cfg.set_mode"), idx: 4 },
          { name: $_("app.navTab.main.firmware"), idx: 5 },
          { name: "Log", idx: 6 },
        ]}
        name="settings"
      />
    </div>
    {#if !tabIdxSettings}
      <PaneMcuSettings />
    {:else if tabIdxSettings === 1}
      <div class="navtab-sub2">
        <NavTabs
          nav_tabs={[
            { name: $_("app.navTab.sender.aliases"), idx: 0 },
            { name: $_("app.navTab.sender.repeater"), idx: 1 },
            { name: $_("app.navTab.sender.transmit"), idx: 2 },
          ]}
          name="sender"
        />
      </div>
      {#if !tabIdxSender}
        <PaneShutterAlias />
      {:else if tabIdxSender === 1}
        <PaneRepeater />
      {:else if tabIdxSender === 2}
        Transmit
      {/if}
    {:else if tabIdxSettings === 2}
      <div class="navtab-sub2">
        <NavTabs
          nav_tabs={[
            { name: $_("app.navTab.positions.durations"), idx: 0 },
            { name: $_("app.navTab.positions.aliases"), idx: 1 },
          ]}
          name="positions"
        />
      </div>
      {#if !tabIdxPositions}
        <PaneShutterDurations />
      {:else if tabIdxPositions === 1}
        <PaneShutterAlias />
      {:else if tabIdxPositions === 2}
        Transmit
      {/if}
    {:else if tabIdxSettings === 3}
      <PaneShutterName />
    {:else if tabIdxSettings === 4}
      <PaneShutterSetMode />
    {:else if tabIdxSettings === 5}
      <PaneFirmwareEsp32 />
    {:else if tabIdxSettings === 6}
      <PaneAppLog />
    {/if}
  {:else if tabIdxMain === 5}
    <PaneUserLevel />
  {:else if !misc.DISTRO && tabIdxMain === 6}
    <PaneDeveloper />
  {/if}
</div>

<p class="help-text-v1 help-text-v0">no_purge_css</p>
