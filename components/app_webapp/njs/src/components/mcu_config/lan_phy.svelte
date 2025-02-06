<script>
  let { name, value = $bindable() } = $props();
</script>

<select class="config-input" id="cfg_{name}" bind:value={value}>
  <option value="ox32gw">Olimex ESP32 Gateway</option>
  <option value="ox32poe">Olimex ESP32 POE</option>
  <option value="wt32eth01">WT32-ETH01</option>
  <option value="lan8720">PHY LAN8720</option>
  <option value="rtl8201">PHY RTL8201</option>
  <option value="ip101">PHY IP101</option>
</select>