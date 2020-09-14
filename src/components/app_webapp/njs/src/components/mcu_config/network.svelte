<script>
  export let name;
  export let value;
</script>

<select class="config-input" id="cfg_{name}" {value}>
  <option value="none">No Network</option>
  <option value="wlan">Existing WLAN</option>
  <option value="ap">WLAN Accesspoint</option>
  <option value="lan">Ethernet</option>
</select>
