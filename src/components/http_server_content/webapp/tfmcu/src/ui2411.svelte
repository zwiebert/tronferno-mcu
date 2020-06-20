<script>
  "use strict";
  import { G, M, M0, GM, Pct, Name } from "./store/curr_shutter.js";
  import { Gmu } from "./store/mcu_config.js";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";
  import DisplayIcons from "./display_icons.svelte";

  export let gmc_fetch_mask = 0;

  let on_destroy = [];
  onMount(() => {
    on_destroy.push(GM.subscribe(() => gmChanged()));
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: name = $Name || "";
  $: vm = $G ? ($M0 ? $M0 : "A") : "";
  $: vg = $G ? $G : "A";

  function shn_fromHtml_toMcu(val) {
    let tfmcu = { to: "tfmcu", shpref: { g: $G, m: $M0, "tag.NAME": val } };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
  }

  function gmChanged() {
    httpFetch.http_fetchByMask(
      httpFetch.FETCH_AUTO |
        httpFetch.FETCH_POS |
        httpFetch.FETCH_SHUTTER_NAME |
        gmc_fetch_mask
    );
  }

  function hClick_G() {
    let g = $G;
    const g_max = 7;
    for (g = g + 1; g <= 7; ++g) {
      if ($Gmu[g]) {
        break;
      }
    }
    if (g > g_max) {
      g = 0;
    }

    $G = g;
  }

  function hClick_M() {
    let m = $M + 1;
    let m_max = $Gmu[$G];
    if (m > m_max) m = 0;

    $M = m;
  }

  function hClick_P() {
    onPos(document.getElementById("spi").value);
  }

  function hClick_Stop() {
    httpFetch.http_postShutterCommand("stop");
  }

  function hClick_Up() {
    httpFetch.http_postShutterCommand("up");
  }

  function hClick_Down() {
    httpFetch.http_postShutterCommand("down");
  }

  function hClick_Sun() {
    httpFetch.http_postShutterCommand("sun-down");
  }

  function hChange_Name() {
    shn_fromHtml_toMcu(name);
  }

  function hChange_Pos() {
    onPos(document.getElementById("spr").value);
  }

  function onPos(pct) {
    let tfmcu = { to: "tfmcu" };
    tfmcu.send = {
      g: $G,
      m: $M,
      p: pct,
    };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
  }
</script>

<style type="text/scss">
  .ft2411-sbt {
    font-size: 60%;
    font-stretch: ultra-condensed;
    height: 4em;
    border-radius: 100%;
  }

  .ft2411-udbt {
    font-size: 125%;
    border-radius: 100%;
  }

  .ft2411-mbt {
  padding: 30px 4px;
  border-radius: 100%;
}
</style>

<div id="sdi" class="inline-block">
  <div class="text-gray-800 bg-gray-400 p-1 font-mono text-lg">

    <div class="h-6 text-center">{name}</div>

    <div class="flex">
      <div class="h-6 w-4">{vg}</div>
      <DisplayIcons />
      <div class="h-6 w-4">{vm}</div>
    </div>

  </div>

  <div class="flex justify-between">
    <button
      class=""
      style="border-radius: 40%;"
      type="button"
      on:click={hClick_G}>
      G
    </button>
    <button
      class=""
      style="border-radius: 40%;"
      type="button"
      on:click={hClick_M}>
      E
    </button>
  </div>

<div class="flex flex-col items-center">

    <button id="sub" class="w-32 ft2411-udbt" type="button" on:click={hClick_Up}>
      &#x25b2;
    </button>

<div class="flex flex-row content-between items-center">
    <button
      class="m-2 ft2411-mbt"
      type="button">
      M
    </button>

    <button
      class="m-2 ft2411-sbt"
      type="button"
      on:click={hClick_Stop}>
      STOP
    </button>

    <button
      class="m-2 ft2411-mbt"
      type="button"
      >
      OK
    </button>

</div>

    <button
      class="w-32 ft2411-udbt"
      type="button"
      on:click={hClick_Down}>
      &#x25bc;
    </button>

</div>

</div>
