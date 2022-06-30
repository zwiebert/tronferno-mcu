<script>
  "use strict";
  import { _ } from "services/i18n";
  import tippy from "sveltejs-tippy";
  import * as appDebug from "app/app_debug.js";
  import * as httpFetch from "app/fetch.js";
  import { GuiAcc } from "stores/app_state";
  import { AutoData, AutoSunEnabled, G, M, M0, GM, GMH } from "stores/curr_shutter.js";
  import { onMount } from "svelte";

  onMount(() => {});

  $: {
    $GM;
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO | httpFetch.FETCH_SHUTTER_NAME);
  }

  $: providTimeString = false;
  $: isoTimeString = new Date().toISOString().substr(0, 19);

  $: autoData = $AutoData;

  $: transmitCountDown = 0;

  let weekly_objArr = weekly_autoData2objArr();

  $: {
    upd_weekly_objArr($AutoData.weekly);
  }

  function upd_weekly_objArr(ws) {
    weekly_objArr = weekly_autoData2objArr(ws);
  }
  //$: weekly_objArr = weekly_autoData2objArr($AutoData.weekly);

  function weekly_objArr2autoData(objArr) {
    let res = "";
    for (let i = 0; i < 7; ++i) {
      let o = objArr[i];
      if (!o.enabled) {
        res += "+";
      } else {
        let tdu = o.up;
        let tdd = o.down;
        let td = tdu.length !== 5 ? "-" : tdu.substring(0, 2) + tdu.substring(3, 5);
        td += tdd.length !== 5 ? "-" : tdd.substring(0, 2) + tdd.substring(3, 5);

        res += td;
      }
    }

    return res;
  }

  function weekly_autoData2objArr(ws) {
    let res = [
      { up: "", down: "", enabled: true },
      { up: "", down: "", enabled: false },
      { up: "", down: "", enabled: false },
      { up: "", down: "", enabled: false },
      { up: "", down: "", enabled: false },
      { up: "", down: "", enabled: false },
      { up: "", down: "", enabled: false },
    ];

    if (!ws) return res;

    let crs = 0;
    for (let wd = 0; wd < 7; ++wd) {
      for (let dir = 0; dir < 2; ++dir) {
        if (ws[crs] === "+") {
          res[wd].enabled = false;
          crs += 1;
          break;
        } else if (ws[crs] === "-") {
          res[wd].enabled = true;
          crs += 1;
          if (dir === 0) {
            res[wd].up = "";
          } else {
            res[wd].down = "";
          }
        } else {
          res[wd].enabled = true;
          let ts = ws.substr(crs, 2) + ":" + ws.substr(crs + 2, 2);
          console.log("ts: ", ts);
          crs += 4;
          if (dir === 0) {
            res[wd].up = ts;
          } else {
            res[wd].down = ts;
          }
        }
      }
    }
    console.log("resoa: ", res);
    return res;
  }

  $: {
    //autoData.weekly = weekly_objArr2autoData(weekly_objArr);
  }

  function hClick_Reload() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
  }

  function hClick_Save() {
    console.log("weekly_objArr: ", weekly_objArr);
    autoData.weekly = weekly_objArr2autoData(weekly_objArr);
    console.log("autoData.weekly1: ", autoData.weekly);
    const reWeekly = /.*[0-9].*/g;
    if (!reWeekly.test(autoData.weekly)) {
      delete autoData.weekly;
      autoData.hasWeekly = false;
    }
    console.log("autoData.weekly2: ", autoData.weekly);
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
    let tfmcu = { to: "tfmcu", auto: {} };
    let auto = tfmcu.auto;
    const ad = autoData;
    auto.g = $G;
    auto.m = $M;

    if (providTimeString) auto.rtc = isoTimeString;

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
    httpFetch.http_postCommand(tfmcu);
  }
</script>

<div id="autodiv" class="auto">
  <table class="top_table">
    <tr>
      <td use:tippy={{ content: $_("app.auto.tt.daily") }}>{$_("app.auto.daily")}</td>
      <td>
        <input class="cb" type="checkbox" bind:checked={autoData.hasDaily} />
      </td>
      <td>
        <div class={autoData.hasDaily ? "" : "invisible"}>
          <label
            ><input type="time" disabled={!autoData.hasDaily} bind:value={autoData.dailyUp} />
            &#x25b3;</label
          ><br />
          <label
            ><input type="time" disabled={!autoData.hasDaily} bind:value={autoData.dailyDown} />
            &#x25bd;</label
          >
        </div>
      </td>
    </tr>
    <tr>
      <td use:tippy={{ content: $_("app.auto.tt.weekly") }}>{$_("app.auto.weekly")}</td>
      <td>
        <input class="cb" type="checkbox" bind:checked={autoData.hasWeekly} />
      </td>
      <td />
    </tr>

    {#if autoData.hasWeekly}
      {#each weekly_objArr as day, i}
        <tr class="auto-weekday">
          <td class="auto-weekday text-sm text-right">{$_("weekdays_short")[i]}</td>
          <td class="auto-weekday text-sm">
            {#if i > 0}
              <label><input type="checkbox" bind:checked={day.enabled} /></label>
            {/if}
          </td>
          <td class="auto-weekday text-sm">
            {#if day.enabled}
              <label
                ><input type="time" bind:value={day.up} />
                &#x25b3;</label
              ><br />
              <label
                ><input type="time" bind:value={day.down} />
                &#x25bd;</label
              >
            {:else}= {$_("weekdays_short")[i - 1]}{/if}
          </td>
        </tr>
      {/each}
    {/if}

    <tr>
      <td use:tippy={{ content: $_("app.auto.tt.random") }}>{$_("app.auto.random")}</td>
      <td>
        <input class="cb" type="checkbox" bind:checked={autoData.isRandom} />
      </td>
    </tr>

    <tr>
      <td use:tippy={{ content: $_("app.auto.tt.astro") }}>{$_("app.auto.astro")}</td>
      <td>
        <input class="cb" type="checkbox" bind:checked={autoData.hasAstro} />
      </td>
      <td>
        <div class={autoData.hasAstro ? "" : "invisible"}>
          <input style="width:5em;" type="number" min="-90" max="90" disabled={!autoData.hasAstro} bind:value={autoData.astro} />
          {autoData.astroToday}
        </div>
      </td>
    </tr>

    <tr>
      <td use:tippy={{ content: $_("app.auto.tt.sun") }}>{$_("app.auto.sun")}</td>
      <td>
        <input class="cb" type="checkbox" bind:checked={autoData.isSun} />
      </td>
      <td>
        {#if $GuiAcc.shutter_sunpos && $M0 && $AutoSunEnabled}
          <button
            id="sspb"
            class="sb text-sm rounded-full"
            type="button"
            on:click={() => httpFetch.http_postShutterCommand("sun-inst")}
            use:tippy={{ content: $_("app.sun.tt.set_sun_pos") }}
          >
            {$_("app.sun.set_sun_pos")}
          </button>
        {/if}
      </td>
    </tr>

    <tr>
      <td use:tippy={{ content: $_("app.auto.tt.manual") }}>{$_("app.auto.manual")}</td>
      <td>
        <input class="cb" type="checkbox" bind:checked={autoData.isManual} />
      </td>
    </tr>
  </table>

  <br />
  <button id="arlb" class="sb" type="button" on:click={hClick_Reload}>{$_("app.reload")}</button>

  <button
    id="asvb"
    class="sb rounded-l-full rounded-r-full"
    type="button"
    use:tippy={{ content: $_("app.auto.tt.sendData") }}
    disabled={transmitCountDown > 0}
    on:click={hClick_Save}>{transmitCountDown > 0 ? ("-" + transmitCountDown + "-") : $_("app.auto.sendData")}</button
  >
  <hr />
  <div class="text-center">
    <button
      use:tippy={{ content: $_("app.auto.tt.sendRtc") }}
      class="sb rounded-l-full rounded-r-full"
      type="button"
      on:click={() => {
        let a = { auto: { g: $G, m: $M, "rtc-only": 1 } };
        if (providTimeString) a.rtc = isoTimeString;
        httpFetch.http_postCommand(a);
      }}
      >{$_("app.auto.sendRtc")} {$GMH}
    </button>

    {#if $GuiAcc.shutter_time_iso}
      <hr />
      <label use:tippy={{ content: $_("app.auto.tt.sendISO") }}>
        {$_("app.auto.sendISO")}
        <input type="checkbox" bind:checked={providTimeString} />
        <input type="text" bind:value={isoTimeString} disabled={!providTimeString} />
      </label>
    {/if}
  </div>
</div>
