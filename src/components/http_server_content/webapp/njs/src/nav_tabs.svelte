<script>
  "use strict";
  import { TabIdx } from "./store/app_state.js";

  export let name = "nav_tab";
  export let nav_tabs = [];

  function setTabIdx(idx) {
    let obj = {};
    obj[name] = idx;
    TabIdx.update(obj);
  }

</script>

<style type="text/scss">
  /* Style the buttons that are used to open the tab content */

</style>

<div id="tabBar" class="overflow-hidden">
  {#each nav_tabs as tab, i}
    <button
      class="p-0 m-1 tabbt {($TabIdx[name] || 0) === i ? 'is-selected' : ''} text-lg font-medium border-none outline-none"
      id="tabbt{i}"
      on:click={() => setTabIdx(i)}>
      {tab}
    </button>
  {/each}
</div>
