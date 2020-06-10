<script>
  "use strict";
  import { TabIdx } from "./store/app_state.js";
  import { onMount, onDestroy } from "svelte";

  export let name = "nav_tab";
  export let nav_tabs = [];

  function setTabIdx(idx) {
    let obj = {};
    obj[name] = idx;
    TabIdx.update(obj);
    console.log(JSON.stringify($TabIdx));
  }

  let on_destroy = [];
  onMount(() => {});
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });
</script>

<style type="text/scss">
  /* Style the buttons that are used to open the tab content */

  .tab {
    background-color: inherit;
    border: none;
    outline: none;
    cursor: pointer;
    padding: 3px 4px;
    transition: 0.3s;
  }

  .tab {
    overflow: hidden;
    border: 1px solid #ccc;
    background-color: #f1f1f1;
  }

  /* Change background color of buttons on hover */
  .tabbt:hover {
    background-color: #ccc;
  }

  /* Create an active/current tablink class */
  .tabbt:active {
    background-color: hsl(220, 60%, 60%);
  }

  .tabbt {
    border: none;
    outline: none;
    padding: 3x 4px;
    transition: 0.3s;
    overflow: hidden;
  }
  .tabbt-active {
    background-color: hsl(220, 60%, 60%);
  }

  .tabbt-inactive {
    background-color: inherit;
  }
</style>

<div id="tabBar" class="tab">
  {#each nav_tabs as tab, i}
    <button
      class="tabbt {$TabIdx[name] === i ? 'tabbt-active' : 'tabbt-inactive'}"
      id="tabbt{i}"
      on:click={() => setTabIdx(i)}>
      {tab.text}
    </button>
  {/each}
</div>
