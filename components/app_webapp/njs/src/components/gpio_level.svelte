<script>
  import { onMount } from 'svelte';

  import { Gpios } from "../store/gpio";
  import { McuConfig } from "../store/mcu_config.js";
  import * as httpFetch from "../app/fetch.js";
  /**
   * @typedef {Object} Props
   * @property {any} name      gpioN 
   * @property {boolean} [force]   
   */

  /** @type {Props} */
  let { name, force = false } = $props();
  //export let value;

  let level = $derived($Gpios[name] || -1);
  let cfg = $derived($McuConfig[name] || "x");
  let output_only = $derived(!(force || /[Qiq]/.test(cfg)));
  
  onMount(() => {
      fetch_level(name);
  });

  $effect(() => {
    if (force || /[Qiq]/.test(cfg)) {
      fetch_level(name);
    }
  });

function fetch_level(gpio_name) {
      let cmd = { mcu: { } };
      cmd.mcu[gpio_name] = "?";
      httpFetch.http_postCommand(cmd);
}

</script>

{#if output_only}
<span class="text-black">&#x25CB</span>
{:else}
  {#if level > 0}
  <span class="text-red-700">&#x25D3</span>
  {:else}
  <span class="text-blue-700">&#x25D2</span>
  {/if}

{/if}



