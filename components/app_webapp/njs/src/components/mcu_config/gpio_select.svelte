<script>
  import { McuGpiosFree } from "../../store/mcu_config.js";

  /**
   * @typedef {Object} Props
   * @property {any} name
   * @property {any} [value]
   * @property {number} [max]
   * @property {boolean} [show_value]
   * @property {boolean} [disabled]
   */

  /** @type {Props} */
  let {
    name,
    value = $bindable(-1),
    max = 64,
    show_value = true,
    disabled = false
  } = $props();
</script>

<select class="config-input" id="cfg_{name}" bind:value disabled={disabled}>
  <option value={-1}>None</option>
  {#if value !== -1 && show_value}
    <option {value}>gpio{value}</option>
  {/if}
  {#each $McuGpiosFree as i}
    {#if i <= max}
      <option value={i}>gpio{i}</option>
    {/if}
  {/each}
</select>
