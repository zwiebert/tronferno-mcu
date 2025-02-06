<script>
  "use strict";
  import { TabIdx } from "../store/app_state.js";

  /**
   * @typedef {Object} Props
   * @property {string} [name]
   * @property {any} [nav_tabs]
   * @property {boolean} [vertical]
   */

  /** @type {Props} */
  let { name = "nav_tab", nav_tabs = [], vertical = false } = $props();

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
      onclick={() => setTabIdx(tab.idx)}>
      {tab.name}
    </button>
    {#if vertical}<br />{/if}
  {/each}
</div>
