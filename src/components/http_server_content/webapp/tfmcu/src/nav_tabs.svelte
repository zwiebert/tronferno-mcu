<script>
  "use strict";
  import { TabIdx } from "./store/app_state.js";
  import { onMount, onDestroy } from "svelte";

  export let nav_tabs = [];

  let on_destroy = [];
  onMount(() => {
 
  });
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
    padding: 14px 16px;
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

  @media only screen and (max-resolution: 96dpi) {
    .tabbt {
      font-size: 200%;
    }
  }
  @media (min-width: 1000px) {
    .tabbt {
      font-size: 100%;
    }
  }

  .tabbt {
    border:none;
    outline:none;
    padding: 14px 16px;
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
    <button class="tabbt {$TabIdx === i ? 'tabbt-active' : 'tabbt-inactive'}" id="tabbt{i}" on:click={() => TabIdx.set(i)}>
      {tab.text}
    </button>
  {/each}
</div>
