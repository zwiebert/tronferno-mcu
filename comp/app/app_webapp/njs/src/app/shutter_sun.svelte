<script>
  "use strict";
  import { GM, M0, Auto, RadioCodeEnabled, AutoSunEnabled } from "stores/curr_shutter.js";
  import * as httpFetch from "app/fetch.js";
  import { onMount, onDestroy } from "svelte";
  import { GuiAcc } from "stores/app_state";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: {
    $GM;
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
  }

  function hClick_Sun() {
    httpFetch.http_postShutterCommand("sun-down");
  }

  function hClick_SunPos() {
    httpFetch.http_postShutterCommand("sun-inst");
  }
</script>

<div id="sdi" class="inline-block">
  <div class="flex flex-row items-center content-between">
    <button id="sspb" class="sb" type="button" on:click={hClick_Sun} use:tippy={{ content: $_("app.sun.tt.move_sun_down") }}>
      {$_("app.sun.move_sun_down")}
    </button>

    {#if $GuiAcc.shutter_sunpos && ($M0 || $RadioCodeEnabled)}
      <button id="sspb" class="sb" type="button" on:click={hClick_SunPos} use:tippy={{ content: $_("app.sun.tt.set_sun_pos") }}>
        {$_("app.sun.set_sun_pos")}
      </button>
    {/if}
  </div>
</div>

<style lang="scss">
</style>
