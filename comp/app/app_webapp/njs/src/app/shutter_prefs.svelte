<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as httpFetch from "app/fetch.js";
  import { G, M, GM, PrefMvut, PrefMvdt, PrefMvspdt } from "stores/curr_shutter.js";
  import { ShowHelp } from "stores/app_state.js";
  import { onMount, onDestroy } from "svelte";
  import tippy from "sveltejs-tippy";

  export const UP = 0;
  export const DOWN = 1;
  export const SUN_DOWN = 2;

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_PREFS);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: mvut = $PrefMvut / 10.0;
  $: mvdt = $PrefMvdt / 10.0;
  $: mvspdt = $PrefMvspdt / 10.0;

  $: {
    $GM;
    httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_PREFS);
  }

  function shp_updHtml() {
    mvut = $PrefMvut / 10.0;
    mvdt = $PrefMvdt / 10.0;
    mvspdt = $PrefMvspdt / 10.0;
  }

  function hClick_Reload() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_PREFS);
    shp_updHtml();
  }

  function hClick_Save() {
    shp_fromHtml_toMcu();
  }

  function hClick_Up() {
    shp_stopClock_do(UP);
  }

  function hClick_Down() {
    shp_stopClock_do(DOWN);
  }

  function hClick_Sun() {
    shp_stopClock_do(SUN_DOWN);
  }

  function shp_fromHtml_toMcu() {
    let tfmcu = { to: "tfmcu", shpref: { g: $G, m: $M, c: "store" } };
    let pref = tfmcu.shpref;

    pref.mvut = Math.floor(parseFloat(mvut) * 10).toString();
    pref.mvdt = Math.floor(parseFloat(mvdt) * 10).toString();
    pref.mvspdt = Math.floor(parseFloat(mvspdt) * 10).toString();

    httpFetch.http_postCommand(tfmcu);
  }

  let stopClock = {
    ms: 100,
    direction: 0,
    ivId: 0,
    text_elem: null,
    val: 0,
  };

  function shp_stopClock_tick() {
    let spsc = stopClock;
    spsc.val += spsc.ms / 100;
    let secs = spsc.val / 10.0;

    if (spsc.direction === UP) {
      mvut = secs;
    } else if (spsc.direction === DOWN) {
      mvdt = secs;
    } else {
      mvspdt = secs;
    }
  }

  function shp_stopClock_do(direction) {
    let spsc = stopClock;

    if (spsc.ivId) {
      shp_stopClock_stop();
      return;
    }

    if (direction === UP) {
      spsc.direction = UP;
      httpFetch.http_postShutterCommand("up");
    } else if (direction === DOWN) {
      spsc.direction = DOWN;
      httpFetch.http_postShutterCommand("down");
    } else if (direction === SUN_DOWN) {
      spsc.direction = SUN_DOWN;
      httpFetch.http_postShutterCommand("sun-down");
    } else {
      return;
    }

    if (!spsc.ivId) {
      spsc.val = 0;
      spsc.ivId = setInterval(shp_stopClock_tick, spsc.ms);
    }
  }

  function shp_stopClock_stop() {
    let spsc = stopClock;
    if (spsc.ivId) {
      clearInterval(spsc.ivId);
      spsc.ivId = 0;
    }
  }
</script>

<div id="shprefdiv">
  <table class="dur_table top_table p-2">
    <tr>
      <th>{$_("app.direction")}</th>
      <th>{$_("app.duration")}</th>
      <th use:tippy={{ content: $_("help.hint_durStopClock") }}>{$_("app.stopClock")}</th>
    </tr>
    <tr>
      <td>{$_("app.upTo100")}</td>
      <td>
        <input class="w-20" id="shpMvut" type="number" min="0" bind:value={mvut} step="0.1" />
      </td>
      <td>
        <button id="shp_MvutButton" on:click={hClick_Up}>Start/Stop</button>
      </td>
    </tr>
    <tr>
      <td>{$_("app.downToZero")}</td>
      <td>
        <input class="w-20" id="shpMvdt" type="number" min="0" bind:value={mvdt} step="0.1" />
      </td>
      <td>
        <button id="shp_MvdtButton" on:click={hClick_Down}>Start/Stop</button>
      </td>
    </tr>
    <tr>
      <td>{$_("app.downSunPos")}</td>
      <td>
        <input class="w-20" id="shpSpMvdt" type="number" min="0" bind:value={mvspdt} step="0.1" />
      </td>
      <td>
        <button id="shp_SpMvdtButton" on:click={hClick_Sun}>Start/Stop</button>
      </td>
    </tr>
  </table>

  <button id="shp_reload" type="button" on:click={hClick_Reload}>{$_("app.reload")}</button>
  <button id="shp_save" type="button" on:click={hClick_Save}>{$_("app.save")}</button>
</div>

<style lang="scss">
</style>
