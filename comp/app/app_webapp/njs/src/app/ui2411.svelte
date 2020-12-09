<script>
  "use strict";
  import { G, M, M0, GM, Pct, Name } from "stores/curr_shutter.js";
  import { Gmu } from "stores/mcu_config.js";
  import { MenuCount, MenuCountMax } from "stores/ui2411.js";
  import * as httpFetch from "app/fetch.js";
  import DisplayIcons from "app/display_icons.svelte";



  $: name = $Name || "";
  $: vm = $G ? ($M0 ? $M0 : "A") : "";
  $: vg = $G ? $G : "A";

  function shn_fromHtml_toMcu(val) {
    let tfmcu = { to: "tfmcu", shpref: { g: $G, m: $M0, "tag.NAME": val } };

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, tfmcu);
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

 

  function hClick_Menu() {
    if ($MenuCount === $MenuCountMax)
     $MenuCount = 0;
     else
     $MenuCount += 1;
  }

  function hClick_OK() {
    $MenuCount = 0;
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
  $color_bg: white;
  $color_fg: black;
  $color_border: grey;

  button {
    background-color: $color_bg;
    border: thin solid $color_border;
    color: $color_fg;
  }

  button:active {
  background-color: darken($color_bg, 5%);
  color: lighten($color_fg, 5%);
  }

  .ft2411-sbt {
    font-size: 100%;
    font-stretch: ultra-condensed;
    height: 4rem;
    width: 4rem;
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
  <div class="text-gray-800 bg-gray-400 p-1 font-mono text-lg rounded-sm">

    <div class="h-6 text-center">{name}</div>

    <div class="flex justify-between">
      <div class="h-6 w-4">{vg}</div>
      <DisplayIcons />
      <div class="h-6 w-4">{vm}</div>
    </div>

  </div>

  <div class="flex justify-between">
    <button
      class="text-lg  pt-1 pb-2 pl-2 pr-4"
      style="border-bottom-right-radius: 100%;"
      type="button"
      on:click={hClick_G}>
      G
    </button>
    <button
      class="text-lg pt-1 pb-2  pl-4 pr-2"
      style="border-bottom-left-radius: 100%;"
      type="button"
      on:click={hClick_M}>
      E
    </button>
  </div>

<div class="flex flex-col items-center">

    <button id="sub" class="w-32 h-10 ft2411-udbt mt-4" type="button" on:click={hClick_Up}>
      &#x25b2;
    </button>

<div class="flex flex-row content-between items-center">
    <button
      class="h-24 w-8 m-2 mr-6 ft2411-mbt"
      type="button"
      on:click={hClick_Menu}>
      M
    </button>

    <button
      class="m-2 ft2411-sbt"
      type="button"
      on:click={hClick_Stop}>
      STOP
    </button>

    <button
      class="h-24 w-8 m-2 ml-6 ft2411-mbt"
      type="button"
      on:click={hClick_OK}>
      OK
    </button>

</div>

    <button
      class="w-32 h-10 ft2411-udbt"
      type="button"
      on:click={hClick_Down}>
      &#x25bc;
    </button>

</div>

</div>
