<script>
  "use strict";
  import { TabIdx } from "../store/app_state.js";

  export let name = "nav_tab";
  export let nav_tabs = [];
  export let vertical = false;

  function setTabIdx(idx) {
    let obj = {};
    obj[name] = idx;
    TabIdx.update(obj);
  }
</script>

<style lang="scss">
  /* Style the buttons that are used to open the tab content */
</style>

<div id="tabBar" class="overflow-hidden">
  {#each nav_tabs as tab}
    <button
      class="p-0 m-1  {($TabIdx[name] || 0) === tab.idx ? 'is-selected' : 'bg-transparent'} text-lg font-medium border-none outline-none tabbt"
      id="tabbt{tab.idx}"
      on:click={() => setTabIdx(tab.idx)}>
      {tab.name}
    </button>
    {#if vertical}<br />{/if}
  {/each}
</div>
