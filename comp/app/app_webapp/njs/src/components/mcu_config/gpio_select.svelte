<script>
  import { McuGpiosFree } from "../../store/mcu_config.js";

  export let name;
  export let value = -1;
  export let max = 64;
  export let show_value = true;
</script>

<select class="config-input" id="cfg_{name}" bind:value>
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
