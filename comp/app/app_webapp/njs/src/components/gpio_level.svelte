<script>
  import { Gpios } from "../store/gpio";
  import { McuConfig } from "stores/mcu_config.js";
  import * as httpFetch from "app/fetch.js";
  export let name;
  //export let value;

  $: level = $Gpios[name] || -1;
  $: cfg = $McuConfig[name] || "x";
  $: output_only = true;

  $: {
    if (/[Qiq]/.test(cfg)) {
      let cmd = { mcu: { } };
      cmd.mcu[name] = "?";
      httpFetch.http_postRequest("/cmd.json", cmd);
      output_only = false;
    } else {
      output_only = true;
    }
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



