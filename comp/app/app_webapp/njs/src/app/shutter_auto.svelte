<script>
  "use strict";
  import { _ } from "services/i18n";
  import * as appDebug from "app/app_debug.js";
  import * as httpFetch from "app/fetch.js";
  import { AutoData, G, M, GM } from "stores/curr_shutter.js";
  import { onMount } from "svelte";

  onMount(() => {});

  $: {
    $GM;
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO | httpFetch.FETCH_SHUTTER_NAME);
  }

  $: autoData = $AutoData;

  $: transmitCountDown = 0;

  function hClick_Reload() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
  }

  function hClick_Save() {
    // disable button for 5 seconds while data is being sent to shutter
    // motor by RF
    transmitCountDown = 5;
    const iv = setInterval(() => {
      if (--transmitCountDown <= 0) clearInterval(iv);
    }, 1000);

    req_automatic();
  }

  // -------------  auto div -------------------------------

  function req_automatic() {
    let url = "/cmd.json";
    let tfmcu = { to: "tfmcu", auto: {} };
    let auto = tfmcu.auto;
    const ad = autoData;
    auto.g = $G;
    auto.m = $M;

    let f = "i";
    f += ad.isManual ? "M" : "m";
    f += ad.hasDaily ? "D" : "d";
    f += ad.hasWeekly ? "W" : "w";
    f += ad.hasAstro ? "A" : "a";
    f += ad.isRandom ? "R" : "r";
    f += ad.isSun ? "S" : "s";
    auto.f = f;

    if (ad.hasWeekly) auto.weekly = ad.weekly;
    if (ad.hasAstro) auto.astro = ad.astro;
    if (ad.hasDaily) {
      let tdu = ad.dailyUp;
      let tdd = ad.dailyDown;
      let td = tdu.length !== 5 ? "-" : tdu.substring(0, 2) + tdu.substring(3, 5);
      td += tdd.length !== 5 ? "-" : tdd.substring(0, 2) + tdd.substring(3, 5);
      auto.daily = td;
    }

    appDebug.dbLog(JSON.stringify(tfmcu));
    httpFetch.http_postRequest(url, tfmcu);
  }
</script>

<style type="text/scss">
  @media only screen and (min-device-width: 361px) {
    #twti {
      width: 30em;
    }
  }
</style>

<div id="autodiv" class="auto">
  <table class="top_table">
    <tr>
      <td>{$_('app.auto.daily')}</td>
      <td><input class="cb" type="checkbox" bind:checked={autoData.hasDaily} /></td>
      <td>
        <label><input type="time" disabled={!autoData.hasDaily} bind:value={autoData.dailyUp} /> &#x25b3;</label><br />
        <label><input type="time" disabled={!autoData.hasDaily} bind:value={autoData.dailyDown} /> &#x25bd;</label>
      </td>
    </tr>
    <tr>
      <td>{$_('app.auto.weekly')}</td>
      <td><input class="cb" type="checkbox" bind:checked={autoData.hasWeekly} /></td>
      <td><input id="twti" type="text" bind:value={autoData.weekly} /></td>
    </tr>
    <tr>
      <td>{$_('app.auto.astro')}</td>
      <td><input class="cb" type="checkbox" bind:checked={autoData.hasAstro} /></td>
      <td>
        <input style="width:5em;" type="number" min="-90" max="90" disabled={!autoData.hasAstro} bind:value={autoData.astro} />
        {autoData.astroToday}
      </td>
    </tr>
    <tr>
      <td>{$_('app.auto.random')}</td>
      <td><input class="cb" type="checkbox" bind:checked={autoData.isRandom} /></td>
    </tr>
    <tr>
      <td>{$_('app.auto.sun')}</td>
      <td><input class="cb" type="checkbox" bind:checked={autoData.isSun} /></td>
    </tr>
    <tr>
      <td>{$_('app.auto.manual')}</td>
      <td><input class="cb" type="checkbox" bind:checked={autoData.isManual} /></td>
    </tr>
  </table>

  <br />
  <button id="arlb" class="sb" type="button" on:click={hClick_Reload}>{$_('app.reload')}</button>
  <button
    id="asvb"
    class="sb"
    type="button"
    disabled={transmitCountDown > 0}
    on:click={hClick_Save}>{transmitCountDown > 0 ? transmitCountDown : $_('app.save')}</button>
  <br />
</div>
