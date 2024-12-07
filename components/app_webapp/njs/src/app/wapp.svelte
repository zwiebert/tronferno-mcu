<script>
  import { _ } from "../services/i18n";
  import * as misc from "../app/misc.js";
  import { GuiAcc, GuiUserLevel } from "../store/app_state";
  import NavTabs from "../app/nav_tabs.svelte";
  import { TabIdx } from "../store/app_state.js";

  import PaneShutterControl from "../panes/shutter_control.svelte";
  import PaneShuttersPct from "../panes/shutters_pct.svelte";
  import PaneShutterAuto from "../panes/shutter_auto.svelte";
  import PaneTransmitterNames from "../panes/transmitter_names.svelte";
  import PaneShutterAlias from "../panes/shutter_alias.svelte";
  import PaneRepeater from "../panes/repeater.svelte";
  import PaneShutterDurations from "../panes/shutter_durations.svelte";
  import PaneShutterName from "../panes/shutter_name.svelte";
  import PaneMcuSettings from "../panes/mcu_settings.svelte";
  import PaneReceiverProgramming from "../panes/receiver_programming.svelte";
  import Pane2411 from "../panes/2411.svelte";
  import PaneAppLog from "../panes/app_log.svelte";
  import PaneDeveloper from "../panes/developer.svelte";
  import PaneUserLevel from "../panes/user_level.svelte";
  import PaneUserHelp from "../panes/user_help.svelte";
  import PaneBackup from "../panes/backup.svelte";
  import PaneHelpDoc from "../panes/help_doc.svelte";
  import PaneHelpFeedback from "../panes/help_feedback.svelte";
  import PaneFirmwareEsp32 from "../panes/firmware_esp32.svelte";
  import PaneHSConfigHomeAssistant from "../panes/hsconfig_home_assistant.svelte";
  import PaneHSConfigFHEM from "../panes/hsconfig_fhem.svelte";
  import PaneHSConfigOpenHAB from "../panes/hsconfig_openhab.svelte";
  import * as httpFetch from "../app/fetch.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_SETTINGS_ALL);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  let tabIdxMain = $derived($TabIdx["main"] || 0);
  let tabIdxSettings = $derived($TabIdx["settings"] || 0);
  let tabIdxSender = $derived($TabIdx["sender"] || 0);
  let tabIdxPositions = $derived($TabIdx["positions"] || 0);
  let tabIdxUserHelp = $derived($TabIdx["user_help"] || 0);
  let tabIdxHSConfig = $derived($TabIdx["hs_config"] || 0);

  function getUserLevelHeader(ul) {
    return ul < 0
      ? $_("navTab.main.user_level.developer")
      : ul < 10
      ? $_("navTab.main.user_level.expert")
      : ul < 20
      ? $_("navTab.main.user_level.admin")
      : ul < 30
      ? $_("navTab.main.user_level.user")
      : $_("navTab.main.user_level.kid");
  }
</script>

<div class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
    <NavTabs
      nav_tabs={[
        { name: $_("navTab.main.move"), idx: 0 },
        { name: "2411", idx: 1 },
        { name: $_("navTab.main.percent"), idx: 2 },
        ...($GuiAcc.shutter_auto ? [{ name: $_("navTab.main.auto"), idx: 3 }] : []),
        ...($GuiAcc.cfg ? [{ name: $_("navTab.main.config"), idx: 4 }] : []),
        { name: $_("navTab.help.tab"), idx: 7 },
        { name: getUserLevelHeader($GuiUserLevel), idx: 5 },
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
          { name: $_("navTab.cfg.mcu.tab"), idx: 0 },
          { name: $_("navTab.cfg.receiver.tab"), idx: 7 },
          { name: $_("navTab.cfg.transmitter.tab"), idx: 1 },
          { name: "Log", idx: 6 },
        ]}
        name="settings"
      />
    </div>
    {#if !tabIdxSettings}
      <PaneMcuSettings />
    {:else if tabIdxSettings === 7}
      <PaneReceiverProgramming />
    {:else if tabIdxSettings === 1}
      <div class="navtab-sub2">
        <NavTabs
          nav_tabs={[
            ...($GuiAcc.edit_transmitter_names ? [{ name: $_("navTab.cfg.transmitter.names.tab"), idx: 0 }] : []),
            { name: $_("navTab.cfg.transmitter.register.tab"), idx: 1 },
            ...($GuiAcc.rf_repeater ? [{ name: $_("navTab.cfg.transmitter.repeater.tab"), idx: 2 }] : []),
            // { name: $_("navTab.cfg.transmitter.transmit"), idx: 2 },
          ]}
          name="sender"
        />
      </div>
      {#if !tabIdxSender}
        <PaneTransmitterNames />
      {:else if tabIdxSender === 1}
        <PaneShutterAlias />
      {:else if tabIdxSender === 2}
        <PaneRepeater />
      {/if}
    {:else if tabIdxSettings === 2}
      <div class="navtab-sub2">
        <NavTabs
          nav_tabs={[
            { name: $_("navTab.positions.durations"), idx: 0 },
            //  { name: $_("navTab.positions.aliases"), idx: 1 },
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
    {:else if tabIdxSettings === 6}
      <PaneAppLog />
    {/if}
  {:else if tabIdxMain === 5}
    <PaneUserLevel />
  {:else if tabIdxMain === 7}
    <div class="navtab-sub">
      <NavTabs nav_tabs={
      [
        ...($GuiAcc.ota ? [{ name: $_("navTab.help.ota.tab"), idx: 0 },] : []),
        ...($GuiAcc.hsc ? [{ name: $_("navTab.help.hsc.tab"), idx: 1 },] : []),
        ...($GuiAcc.bak ? [{ name: $_("navTab.help.backup.tab"), idx: 2 },] : []),
        { name: $_("navTab.help.doc.tab"), idx: 3 },
        { name: $_("navTab.help.feedback.tab"), idx: 4 },
      ]}
       name="user_help" />
    </div>
    {#if !tabIdxUserHelp}
      <PaneFirmwareEsp32 />
    {:else if $GuiAcc.hsc && tabIdxUserHelp === 1}
    <div class="navtab-sub">
      <NavTabs nav_tabs={
      [
        { name: "Home Assistant", idx: 0 },
        { name: "FHEM", idx: 1 },
        { name: "OpenHAB", idx: 2 },
      ]}
       name="hs_config" />
    </div>
      {#if !tabIdxHSConfig}
        <PaneHSConfigHomeAssistant />
      {:else if tabIdxHSConfig === 1}
        <PaneHSConfigFHEM />
      {:else if tabIdxHSConfig === 2}
        <PaneHSConfigOpenHAB />
      {/if}
    {:else if tabIdxUserHelp === 2}
      <PaneBackup />
    {:else if tabIdxUserHelp === 3}
      <PaneHelpDoc />
    {:else if tabIdxUserHelp === 4}
      <PaneHelpFeedback />
    {/if}
  {:else if !misc.DISTRO && tabIdxMain === 6}
    <PaneDeveloper />
  {/if}
</div>

<p class="help-text-v1 help-text-v0">no_purge_css</p>
