<script>
  'use strict';
  import {G,M,M0,GM, Pct, Name} from "stores/curr_shutter.js";
  import { Gmu } from "stores/mcu_config.js";
  import * as httpFetch from "app/fetch.js";
  import { onMount,onDestroy } from "svelte";
  import DisplayIcons from "app/display_icons.svelte";

let on_destroy = [];
onMount(() => {
     on_destroy.push(GM.subscribe(() => gmChanged()));
  });
onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
});

 
  $: name = $Name || '';
  $: vm = $G ? ($M0 ? $M0 : 'A') : '';
  $: vg = $G ? $G : 'A';

 function shn_fromHtml_toMcu(val) {
  let tfmcu = { "to":"tfmcu", "shpref":{"g":$G, "m":$M0, "tag.NAME":val }};

  let url = '/cmd.json';
    httpFetch.http_postRequest(url, tfmcu);
  }

  function gmChanged() {
      httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO | httpFetch.FETCH_POS | httpFetch.FETCH_SHUTTER_NAME);
  }

  function hClick_G() {
    let g = $G;
    const g_max = 7;
    for (g=g+1; g <= 7; ++g) {
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
    if (m > m_max)
      m = 0;

    $M = m;
  }
  
  function hClick_P() {
    onPos(document.getElementById("spi").value);
  }

  function hClick_Stop() {
    httpFetch.http_postShutterCommand('stop');
  }
  
  function hClick_Up() {
    httpFetch.http_postShutterCommand('up');
  }
  
  function hClick_Down() {
    httpFetch.http_postShutterCommand('down');
  }
  
  function hClick_Sun() {
    httpFetch.http_postShutterCommand('sun-down');
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

  let url = '/cmd.json';
  httpFetch.http_postRequest(url, tfmcu);
  }


</script>

<style type="text/scss">

      #senddiv {
      display: grid;
      width: 100%;
      grid-template-columns: 6rem 6rem 6rem;
      grid-template-rows: repeat(8, 1fr);
      grid-template-areas:
      "mn  mn  mn"
      "di  di  di"
      "pr  pr  pr"
      "gi  pi  mi"
      "gb  pl  mb"
      ".   ub  . "
      ".   sb  ."
      "pb  db  ."
      ;
      justify-items: center;
      align-items: center;
      grid-gap: 2px;
      }
      
      .sb {width: 5em; margin: 2px; }

      #sgb, #smb { border-radius: 40%; }
      #sgi, #spi, #smi { width: 2.5em; height: 2em; text-align: center; }
      #sgb, #smb { width: 2em; height: 2em; }
      #sprXXX { height: 3em; }

      #ssb { font-size: 60%;  font-stretch: ultra-condensed; height: 4em; border-radius: 100% }
      #sub, #sdb { font-size: 125%; border-radius: 100% }

      #smn { grid-area: mn; }
      #sgi { grid-area: gi; }
      #sgb { grid-area: gb; }
      #spi { grid-area: pi; }
      #spb { grid-area: pl; }
      #smi { grid-area: mi; }
      #smb { grid-area: mb; }
      #sub { grid-area: ub; }
      #ssb { grid-area: sb; }
      #sdb { grid-area: db; }
      #spr { grid-area: pr; }
      #sspb { grid-area: pb; }
      #sdi { grid-area: di; }
      
  


</style>

<div id="senddiv" class="send full-width">
  <input id="smn" type = "text" name = "name" bind:value="{name}" on:change={hChange_Name}>
  <input id="sgi" type = "text" name = "g" value="{vg}">
  <input id="spi" type = "number" min="0" max="100" name = "p" value="{$Pct}">
  <input id="spr" type = "range" min="0" max="100" name = "p" value="{$Pct}" on:change={hChange_Pos}>
  <input id="smi" type = "text" name = "m" value="{vm}">
  <br>
  <button id="sgb" type="button" on:click={hClick_G}>G</button>
  <button id="spb" type="button" on:click={hClick_P}>Position</button>
  <button id="smb" type="button" on:click={hClick_M}>E</button>
  <br><button id="sub" class="sb" type="button" on:click={hClick_Up}>&#x25b2;</button>
  <br><button id="ssb" class="sb" type="button" on:click={hClick_Stop}>STOP</button>
  <br><button id="sdb" class="sb" type="button" on:click={hClick_Down}>&#x25bc;</button>
  <br><button id="sspb" class="sb" type="button" on:click={hClick_Sun}>Sun</button>

  <div id="sdi" class="flex bg-gray-400 p-1">
    <div class="h-4 w-4 text-gray-800">
     {vg}
    </div>

  <DisplayIcons/>
      <div class="h-4 w-4 text-gray-800">
     {vm}
    </div>
  </div>

</div>

