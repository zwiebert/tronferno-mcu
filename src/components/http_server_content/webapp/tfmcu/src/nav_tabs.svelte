<script>
'use strict';
import {TabIdx } from './store/app_state.js';
import { onMount,onDestroy } from 'svelte';
import * as httpFetch from './fetch.js';
import { GmcFetch} from './store/app_state.js';

export let nav_tabs = [];

let on_destroy = [];
onMount(() => {
    on_destroy.push(TabIdx.subscribe(idx => navTabs_updHtml(idx)));
  });
onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
});


 function navTabs_updHtml(idx) {
  const NONE = "none";
  const SHOW = "";
  const BGC1 = "hsl(220, 60%, 60%)";
  const BGC0 = "#eee";
console.log("tidxupd: ", idx);
  if (!(0 <= idx && idx < tabs.length))
    idx = 0;

  for (let i = 0; i < tabs.length; ++i) {
    document.getElementById('tabbt' + i.toString()).style.backgroundColor = (i === idx) ? BGC1 : BGC0;
  }

  const nt = tabs[idx];

  let fetch = 0;

  if ('fetch_init' in nt) {
    fetch |= nt.fetch_init;
  }
  if ('fetch' in nt) {
    fetch |= nt.fetch;
  }
  if ('fetch_gm' in nt) {
    fetch |= nt.fetch_gm;
    GmcFetch.set(nt.fetch_gm);
  } else {
    GmcFetch.set(0);
  }

  if (fetch) {
    httpFetch.http_fetchByMask(fetch);
  }
}

//--------------- nav tabs ------------------
 
let tabs = [...nav_tabs];

</script>

<style>      /* Style the buttons that are used to open the tab content */

.tab button {
      background-color: inherit;
      float: left;
      border: none;
      outline: none;
      cursor: pointer;
      padding: 14px 16px;
      transition: 0.3s;
      }
      
            .tab {
      overflow: hidden;
      border: 1px solid #ccc;
      background-color: #f1f1f1;
      }



      /* Change background color of buttons on hover */
     .tab button:hover {
      background-color: rgb(197, 185, 185);
      }

      /* Create an active/current tablink class */
    .tab button:active {
      background-color: #ccc;
      }
      

      
      </style>

<div id="tabBar" class="tab">
{#each tabs as tab, i}
<button class="tab" id="tabbt{i}" on:click={() => TabIdx.set(i)}>{tab.text}</button>
{/each}
</div>

 