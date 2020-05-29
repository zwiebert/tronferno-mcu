<script context="module">
'use strict';
import * as appState from './app_state';
import * as httpFetch from './fetch.js';


//--------------- nav tabs ------------------
export const tabs = [
  { 'text': 'Command', 'div_id': ['senddiv'], fetch_gm: (httpFetch.FETCH_POS | httpFetch.FETCH_SHUTTER_NAME) },
  { 'text': 'Automatic', 'div_id': ['senddiv', 'autodiv'], fetch_gm: (httpFetch.FETCH_AUTO | httpFetch.FETCH_POS | httpFetch.FETCH_SHUTTER_NAME) },
  { 'text': 'Config', 'div_id': ['configdiv'], fetch: httpFetch.FETCH_CONFIG },
  { 'text': 'Positions', 'div_id': ['senddiv', 'aliasdiv', 'shprefdiv'], fetch: httpFetch.FETCH_ALIASES, fetch_gm: httpFetch.FETCH_POS |  httpFetch.FETCH_SHUTTER_PREFS | httpFetch.FETCH_SHUTTER_NAME },
  { 'text': 'Firmware', 'div_id': ['id-fwDiv'], fetch_init: (httpFetch.FETCH_VERSION |  httpFetch.FETCH_GIT_TAGS) },
  { 'text': 'Tests', 'div_id': ['testsdiv'], },
];
let div_ids = [];

export function navTabs_updHtml(idx) {
  const NONE = "none";
  const SHOW = "";
  const BGC1 = "hsl(220, 60%, 60%)";
  const BGC0 = "#eee";

  if (!(0 <= idx && idx < tabs.length))
    idx = 0;

  for (let i = 0; i < tabs.length; ++i) {
    document.getElementById('tabbt' + i.toString()).style.backgroundColor = (i === idx) ? BGC1 : BGC0;
  }

  for (let div_id of div_ids) {
    document.getElementById(div_id).style.display = tabs[idx].div_id.includes(div_id) ? SHOW : NONE;
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
    appState.ast.gmc_fetch = nt.fetch_gm;
  } else {
    appState.ast.gmc_fetch = 0;
  }

  if (fetch) {
    httpFetch.http_fetchByMask(fetch);
  }
}

function onNavTab(idx) {
  appState.ast.tabVisibility = idx;
}

function navTabs_init() {
  for (let i = 0; i < tabs.length; ++i) {
    const tab = tabs[i];
    for (let k = 0; k < tabs[i].div_id.length; ++k) {
      const div_id = tabs[i].div_id[k];
      if (!div_ids.includes(div_id)) {
        div_ids.push(div_id);
      }
    }
  }
}

navTabs_init();
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
<button class="tab" id="tabbt{i}" on:click={() => onNavTab(i)}>{tab.text}</button>
{/each}
</div>

  

