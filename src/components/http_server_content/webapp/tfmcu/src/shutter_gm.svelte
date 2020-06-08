<script>
  'use strict';
  import {G,M,M0,GM, Name} from './store/curr_shutter.js';
  import {Names} from './store/shutters.js';
  import { Gmu } from './store/mcu_config.js';
  import * as httpFetch from './fetch.js';
  import { onMount,onDestroy } from 'svelte';

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

  $: gm = $GM;
  $: nameKeys = Object.keys($Names);
  $: names = Object.values($Names);
  $: name = $Name || '';
  $: vm = $G ? ($M0 ? $M0 : 'A') : '';
  $: vg = $G ? $G : 'A';

 function shn_fromHtml_toMcu(val) {
  let tfmcu = { "to":"tfmcu", "shpref":{"g":$G, "m":$M0, "tag.NAME":val }};

  let url = '/cmd.json';
    httpFetch.http_postRequest(url, tfmcu);
  }

  function gmChanged() {
      httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_NAME | gmc_fetch_mask);
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
  
 
  function hChange_Name() {
    $G = Number.parseInt(gm.substr(0,1));
    $M = Number.parseInt(gm.substr(1,1));
  }
  
  
</script>

<style type="text/scss">

      #shugmdiv {
      background-color: #eee;
      display: grid;
      width: 50%;
      grid-template-columns: 3em 3em 12em 3em 3em;
      grid-template-rows: repeat(1, 1fr);
      grid-template-areas:
      "gi  gb mn mb mi"
      ;
      justify-items: center;
      align-items: center;
      grid-gap: 2px;
      }

      #sgb, #smb { border-radius: 40%; }
      #sgi, #smi { width: 2.5em; height: 2em; text-align: center; }
      #sgb, #smb { width: 2em; height: 2em; }
      #smn { width: 10em; height: 2em; text-align: center;}


      #smn { grid-area: mn; }
      #sgi { grid-area: gi; }
      #sgb { grid-area: gb; }
      #smi { grid-area: mi; }
      #smb { grid-area: mb; }

      
</style>

<div id="shugmdiv" >
  <select id="smn" bind:value={gm} on:change={hChange_Name} on:blur={hChange_Name}>
   {#each nameKeys as nk}
     <option value="{nk}">{$Names[nk]}</option>
   {/each}
  </select>
  <input id="sgi" type = "text" name = "g" value="{vg}">
  <input id="smi" type = "text" name = "m" value="{vm}">
  <br>
  <button id="sgb" type="button" on:click={hClick_G}>G</button>
  <button id="smb" type="button" on:click={hClick_M}>E</button>
</div>