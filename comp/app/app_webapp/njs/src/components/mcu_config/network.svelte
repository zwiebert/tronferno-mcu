<script>
  export let name;
  export let value;
  export let has_lan = true;
</script>

<select class="config-input" id="cfg_{name}" bind:value>
  <option value="none">No Network</option>
  <option value="wlan">Existing WLAN</option>
  <option value="ap">WLAN Accesspoint</option>
  {#if has_lan}
    <option value="lan">Ethernet</option>
    <option value="lan-wlan">Ethernet or WLAN</option>
  {/if}
</select>
