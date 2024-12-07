<script>
  "use strict";
  import ShutterSelectGM from "../components/shutter_select_gm.svelte";
  /**
   * @typedef {Object} Props
   * @property {boolean} [radio]
   * @property {boolean} [groups]
   */

  /** @type {Props} */
  let { radio = true, groups = true } = $props();
</script>

<div class="text-center">
  <ShutterSelectGM {radio} {groups} />
</div>
